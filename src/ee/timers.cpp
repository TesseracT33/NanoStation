#include "timers.hpp"
#include "ee.hpp"
#include "intc.hpp"
#include "log.hpp"
#include "scheduler.hpp"

#include <array>
#include <format>
#include <limits>
#include <utility>

namespace ee::timers {

enum Addr : u32 {
    TN_COUNT = 0x1000'0000,
    TN_MODE = 0x1000'0010,
    TN_COMP = 0x1000'0020,
    TN_HOLD = 0x1000'0030,
};

enum class GateMode : u8 {
    CountWhileNotGated,
    ResetCountOnGateTransToHi,
    ResetCountOnGateTransToLo,
    ResetCountOnGateTrans,
};

enum class GateType : u8 {
    HBlank,
    VBlank,
};

struct Timer {
    Timer(u8 index,
      Interrupt intc_interrupt,
      scheduler::EventType match_interrupt_event,
      scheduler::EventType overflow_interrupt_event,
      scheduler::EventType sbus_interrupt_event)
      : intc_interrupt{ intc_interrupt },
        match_interrupt_event{ match_interrupt_event },
        overflow_interrupt_event{ overflow_interrupt_event },
        sbus_interrupt_event{ sbus_interrupt_event },
        index{ index }
    {
        init();
    }

    u64 time_when_counter_refreshed;
    u32 counter_max_excl;
    Interrupt intc_interrupt;
    u16 compare;
    u16 counter;
    u16 counter_rem;
    u16 ee_clock_scale;
    u16 mode_raw;
    u16 sbus_int_compare;
    scheduler::EventType match_interrupt_event;
    scheduler::EventType overflow_interrupt_event;
    scheduler::EventType sbus_interrupt_event;
    GateMode gate_mode;
    GateType gate_type;
    u8 index;
    bool clear_counter_on_compare_match;
    bool compare_int_enable;
    bool compare_int_flag;
    bool enabled;
    bool gate_enable;
    bool gated;
    bool overflow_int_enable;
    bool overflow_int_flag;

    void init();
    bool is_counting() const;
    void on_compare_match();
    void on_gate_event(GateType gate_type, bool active);
    void on_hold_match();
    void on_overflow();
    u16 read_counter();
    void refresh_counter();
    void refresh_counter_max();
    void refresh_events();
    void schedule_compare_event();
    void schedule_overflow_event();
    void schedule_sbus_event();
    void write_compare(u32 value);
    void write_counter(u32 value);
    void write_sbus_counter(u32 value);
    void write_mode(u32 data);
};

static std::array<Timer, 4> timers{
    Timer{
      0,
      Interrupt::Timer0,
      scheduler::EventType::EETimer0Match,
      scheduler::EventType::EETimer0Overflow,
      scheduler::EventType::EETimer0Sbus,
    },
    Timer{
      1,
      Interrupt::Timer1,
      scheduler::EventType::EETimer1Match,
      scheduler::EventType::EETimer1Overflow,
      scheduler::EventType::EETimer1Sbus,
    },
    Timer{
      2,
      Interrupt::Timer2,
      scheduler::EventType::EETimer2Match,
      scheduler::EventType::EETimer2Overflow,
      scheduler::EventType::EETimer0Sbus, // unused
    },
    Timer{
      3,
      Interrupt::Timer3,
      scheduler::EventType::EETimer3Match,
      scheduler::EventType::EETimer3Overflow,
      scheduler::EventType::EETimer0Sbus, // unused
    },
};

void Timer::init()
{
    write_mode(0xc00); // zero everything + clear interrupt flags
}

bool Timer::is_counting() const
{
    return enabled && !gated;
}

void Timer::on_compare_match()
{
    mode_raw |= 0x400;
    if (!std::exchange(compare_int_flag, true) && compare_int_enable) {
        raise_intc(intc_interrupt);
    }
}

void Timer::on_gate_event(GateType gate_type, bool active)
{
    if (gate_enable && this->gate_type == gate_type) {
        switch (gate_mode) {
        case GateMode::CountWhileNotGated:
            gated = active;
            refresh_events();
            break;
        case GateMode::ResetCountOnGateTransToHi:
            if (active) {
                write_counter(0);
            }
            break;
        case GateMode::ResetCountOnGateTransToLo:
            if (!active) {
                write_counter(0);
            }
            break;
        case GateMode::ResetCountOnGateTrans: write_counter(0); break;
        }
    }
}

void Timer::on_hold_match()
{
    raise_intc(Interrupt::SBUS);
    schedule_sbus_event();
}

void Timer::on_overflow()
{
    mode_raw |= 0x800;
    if (!std::exchange(overflow_int_flag, true) && overflow_int_enable) {
        raise_intc(intc_interrupt);
    }
}

u16 Timer::read_counter()
{
    refresh_counter();
    return counter;
}

void Timer::refresh_counter()
{
    u64 ee_time = get_ee_time();
    if (is_counting()) {
        u64 time_elapsed = ee_time - time_when_counter_refreshed;
        counter += u16(time_elapsed / ee_clock_scale);
        counter = u16(counter % counter_max_excl);
        counter_rem += u16(time_elapsed % ee_clock_scale);
        bool rem_overflow = counter_rem >= ee_clock_scale;
        counter += rem_overflow;
        counter_rem -= u16(ee_clock_scale * rem_overflow);
    }
    time_when_counter_refreshed = ee_time;
}

void Timer::refresh_counter_max()
{
    counter_max_excl = clear_counter_on_compare_match ? compare + 1 : 0x10000;
}

void Timer::refresh_events()
{
    if (is_counting()) {
        refresh_counter();
        schedule_compare_event();
        schedule_overflow_event();
        if (index < 2) {
            schedule_sbus_event();
        }
    } else {
        scheduler::remove_event(match_interrupt_event);
        scheduler::remove_event(overflow_interrupt_event);
        if (index < 2) {
            scheduler::remove_event(sbus_interrupt_event);
        }
    }
}

void Timer::schedule_compare_event()
{
    if (compare_int_enable && !compare_int_flag) {
        u32 ticks_until_match = compare - counter - 1;
        if (compare < counter) {
            ticks_until_match += counter_max_excl;
        }
        u32 ee_cycles_until_match = ticks_until_match * ee_clock_scale - counter_rem;
        scheduler::add_event_or_change_time(match_interrupt_event, ee_cycles_until_match, [this] {
            on_compare_match();
        });
    } else {
        scheduler::remove_event(match_interrupt_event);
    }
}

void Timer::schedule_overflow_event()
{
    if (overflow_int_enable && !overflow_int_flag && !clear_counter_on_compare_match) {
        u32 ticks_until_overflow = 0x10000 - counter;
        u32 ee_cycles_until_overflow = ticks_until_overflow * ee_clock_scale - counter_rem;
        scheduler::add_event_or_change_time(overflow_interrupt_event, ee_cycles_until_overflow, [this] {
            on_overflow();
        });
    } else {
        scheduler::remove_event(overflow_interrupt_event);
    }
}

void Timer::schedule_sbus_event()
{
    if (!clear_counter_on_compare_match || sbus_int_compare <= compare || counter > compare) {
        u32 ticks_until_sbus = sbus_int_compare - counter;
        if (sbus_int_compare < counter) {
            ticks_until_sbus += counter_max_excl;
        }
        u32 ee_cycles_until_sbus = ticks_until_sbus * ee_clock_scale - counter_rem;
        scheduler::add_event_or_change_time(sbus_interrupt_event, ee_cycles_until_sbus, [this] { on_hold_match(); });
    } else {
        scheduler::remove_event(sbus_interrupt_event);
    }
}

void Timer::write_compare(u32 value)
{
    u16 new_compare = u16(value);
    if (std::exchange(compare, new_compare) != new_compare && is_counting()) {
        refresh_counter_max();
        refresh_events();
    }
}

void Timer::write_counter(u32 value)
{
    u16 new_counter = u16(value);
    if (std::exchange(counter, new_counter) != new_counter && is_counting()) {
        counter_rem = 0;
        refresh_events();
    }
}

void Timer::write_mode(u32 data)
{
    if ((data ^ mode_raw) & 0x3ff) {
        u32 clock_mode = data & 3;
        if (clock_mode == 3) {
            // PAL:  9436 BUSCLK cycles per scanline
            // NTSC: 9370 BUSCLK cycles per scanline
            ee_clock_scale = 9370 * 2; // TODO: PAL
        } else {
            // 0 => bus clock; 1 => bus clock / 16; 2 => bus clock / 256.
            // ee clock == bus clock * 2
            ee_clock_scale = u16(1 << (4 * clock_mode + 1));
        }
        gate_enable = data & 4;
        gate_type = static_cast<GateType>(data >> 3 & 1);
        gate_mode = static_cast<GateMode>(data >> 4 & 3);
        clear_counter_on_compare_match = data & 0x40;
        enabled = data & 0x80;
        compare_int_enable = data & 0x100;
        overflow_int_enable = data & 0x200;
        counter_rem = 0;
        gated = false; // TODO: what if gate mode is changed during v/hblank?
        refresh_counter_max();
    }
    compare_int_flag &= !(data & 0x400);
    overflow_int_flag &= !(data & 0x800);
    mode_raw = u16(data & 0xf3ff | compare_int_flag << 10 | overflow_int_flag << 11);
    refresh_events();
}

void Timer::write_sbus_counter(u32 value)
{
    if (index < 2) {
        u16 new_compare = u16(value);
        if (std::exchange(sbus_int_compare, new_compare) != new_compare && is_counting()) {
            schedule_sbus_event();
        }
    }
}

void init()
{
    for (Timer& t : timers) {
        t.init();
    }
}

void on_hblank(bool active)
{
    for (Timer& t : timers) {
        t.on_gate_event(GateType::HBlank, active);
    }
}

void on_vblank(bool active)
{
    for (Timer& t : timers) {
        t.on_gate_event(GateType::VBlank, active);
    }
}

u32 read_io(u32 addr)
{
    Timer& timer = timers[addr >> 11 & 3];
    switch (addr & ~0x1800) {
    case Addr::TN_COUNT: return timer.read_counter(); // 10000000h + N*800h
    case Addr::TN_MODE:  return timer.mode_raw; // 10000010h + N*800h
    case Addr::TN_COMP:  return timer.compare; // 10000020h + N*800h
    case Addr::TN_HOLD:  return timer.sbus_int_compare; // 10000030h + N*800h
    default:             log_error("EE: Tried to read from unknown timer IO address: {:08X}", addr); return {};
    }
}

void write_io(u32 addr, u32 data)
{
    Timer& timer = timers[addr >> 11 & 3];
    switch (addr & ~0x1800) {
    case Addr::TN_COUNT: timer.write_counter(data); break; // 10000000h + N*800h
    case Addr::TN_MODE:  timer.write_mode(data); break; // 10000010h + N*800h
    case Addr::TN_COMP:  timer.write_compare(data); break; // 10000020h + N*800h
    case Addr::TN_HOLD:  timer.write_sbus_counter(data); break; // 10000030h + N*800h
    default:             log_error("EE: Tried to write to unknown timer IO address: {:08X}", addr);
    }
}

} // namespace ee::timers
