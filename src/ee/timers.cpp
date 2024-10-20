#include "timers.hpp"
#include "ee.hpp"
#include "frontend/message.hpp"
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
    TN_HOLD = 0x1000'0030
};

enum class GateMode : u8 {
    CountWhileNotGated,
    ResetCountOnGateHi,
    ResetCountOnGateLo,
    ResetCountOnGateTrans
};

enum class GateType : u8 {
    HBlank,
    VBlank
};

struct Timer {
    Timer(u8 index, Interrupt intc_interrupt, scheduler::EventType interrupt_event)
      : index(index),
        intc_interrupt(intc_interrupt),
        interrupt_event(interrupt_event)
    {
        reset();
    }

    u64 time_last_counter_refresh;
    u32 counter_max;
    Interrupt const intc_interrupt;

    u16 compare;
    u16 counter;
    u16 counter_rem;
    u16 ee_clock_scale;
    u16 mode_raw;
    u16 sbus_int_counter;
    scheduler::EventType const interrupt_event;
    GateMode gate_mode;
    GateType gate_type;
    u8 const index;
    bool clear_counter_on_compare_match;
    bool compare_int_enable;
    bool compare_int_flag;
    bool enabled;
    bool gate_enable;
    bool gated;
    bool overflow_int_enable;
    bool overflow_int_flag;

    bool is_counting() const;
    void on_compare_match();
    void on_hold_match();
    void on_overflow();
    u16 read_counter();
    void refresh_counter();
    void reset();
    template<bool check_overflow = true, bool check_compare = true> void handle_events();
    void handle_events_no_compare();
    void handle_events_no_overflow();
    void set_compare(u32 value);
    void set_counter(u32 value);
    void set_sbus_counter(u32 value);
    void update_gating(GateType gate_type, bool active);
    void write_to_mode(u32 data);
};

static std::array<Timer, 4> timers{ Timer{ 0, Interrupt::Timer0, scheduler::EventType::EETimer0Interrupt },
    Timer{ 1, Interrupt::Timer1, scheduler::EventType::EETimer1Interrupt },
    Timer{ 2, Interrupt::Timer2, scheduler::EventType::EETimer2Interrupt },
    Timer{ 3, Interrupt::Timer3, scheduler::EventType::EETimer3Interrupt } };

bool Timer::is_counting() const
{
    return enabled && !gated;
}

void Timer::on_compare_match()
{
    if (clear_counter_on_compare_match) {
        counter = counter_rem = 0;
        time_last_counter_refresh = get_time();
    }
    if (compare_int_enable) {
        if (!std::exchange(compare_int_flag, 1)) {
            mode_raw |= 0x400;
            raise_intc(intc_interrupt);
        }
        handle_events_no_compare();
    }
}

void Timer::on_hold_match()
{
    raise_intc(Interrupt::SBUS);
    handle_events();
}

void Timer::on_overflow()
{
    if (overflow_int_enable && !std::exchange(overflow_int_flag, 1)) {
        mode_raw |= 0x800;
        raise_intc(intc_interrupt);
        handle_events_no_overflow();
    }
}

u16 Timer::read_counter()
{
    refresh_counter();
    return counter;
}

void Timer::refresh_counter()
{
    u64 global_time = get_time();
    if (is_counting()) {
        u64 delta = global_time - time_last_counter_refresh;
        counter_rem += u16(delta % ee_clock_scale);
        bool rem_overflow = counter_rem >= ee_clock_scale;
        counter += u16(delta / ee_clock_scale + rem_overflow);
        counter_rem -= ee_clock_scale * rem_overflow;
    }
    time_last_counter_refresh = global_time;
}

void Timer::reset()
{
    time_last_counter_refresh = {};
    counter_max = 0x10000;
    compare = {};
    counter = {};
    counter_rem = {};
    ee_clock_scale = {};
    mode_raw = {};
    sbus_int_counter = {};
    gate_mode = {};
    gate_type = {};
    clear_counter_on_compare_match = {};
    compare_int_enable = {};
    compare_int_flag = {};
    enabled = {};
    gate_enable = {};
    gated = {};
    overflow_int_enable = {};
    overflow_int_flag = {};
    write_to_mode(0);
}

template<bool check_overflow, bool check_compare> void Timer::handle_events()
{
    // TODO: possibly have multiple timer events per timer
    if (is_counting()) {
        refresh_counter();
        u32 fewest_ticks = std::numeric_limits<u32>::max();
        scheduler::EventCallback handler{};
        if constexpr (check_overflow) {
            if (overflow_int_enable && !overflow_int_flag && (!clear_counter_on_compare_match)) {
                u32 time_until_overflow = counter_max - counter;
                if (time_until_overflow < fewest_ticks) {
                    fewest_ticks = time_until_overflow;
                    handler = {}; // TODO
                }
            }
        }
        if constexpr (check_compare) {
            if (compare_int_enable && !compare_int_flag) {
                u32 time_until_match = compare - counter;
                if (compare < counter) {
                    time_until_match += counter_max;
                }
                if (time_until_match < fewest_ticks) {
                    fewest_ticks = time_until_match;
                    handler = {};
                }
            }
        }
        if (index < 2) {
            u32 time_until_sbus = sbus_int_counter - counter;
            if (sbus_int_counter < counter) {
                time_until_sbus += counter_max;
            }
            if (time_until_sbus < fewest_ticks) {
                fewest_ticks = time_until_sbus;
                handler = {};
            }
        }
        if (handler) {
            scheduler::add_event_or_change_time(interrupt_event, fewest_ticks * ee_clock_scale, handler);
        } else {
            scheduler::remove_event(interrupt_event);
        }
    } else {
        scheduler::remove_event(interrupt_event);
    }
}

void Timer::handle_events_no_compare()
{
    handle_events<true, false>();
}

void Timer::handle_events_no_overflow()
{
    handle_events<false, true>();
}

void Timer::set_compare(u32 value)
{
    compare = value & 0xFFFF;
    if (enabled) {
        if (read_counter() == compare) {
            on_compare_match();
        } else if (compare_int_enable && !compare_int_flag) {
            handle_events();
        }
    }
}

void Timer::set_counter(u32 value)
{
    time_last_counter_refresh = get_time();
    counter = value & 0xFFFF;
    counter_rem = 0;
    if (enabled) {
        counter == compare ? on_compare_match() : handle_events();
    }
}

void Timer::set_sbus_counter(u32 value)
{
    if (index < 2) {
        sbus_int_counter = value & 0xFFFF;
        if (enabled) {
            read_counter() == sbus_int_counter ? on_hold_match() : handle_events();
        }
    }
}

void Timer::update_gating(GateType gate_type, bool active)
{
    if (gate_enable && this->gate_type == gate_type) {
        using enum GateMode;
        switch (gate_mode) {
        case CountWhileNotGated:
            gated = active;
            handle_events();
            break;
        case ResetCountOnGateHi:
            gated = false;
            active ? set_counter(0) : handle_events();
            break;
        case ResetCountOnGateLo:
            gated = false;
            !active ? set_counter(0) : handle_events();
            break;
        case ResetCountOnGateTrans:
            gated = false;
            set_counter(0);
            break;
        }
    }
}

void Timer::write_to_mode(u32 data)
{
    u32 clock_mode = data & 3;
    if (clock_mode == 3) {
        // PAL:  9436 BUSCLK cycles per scanline
        // NTSC: 9370 BUSCLK cycles per scanline
        ee_clock_scale = 9370 * 2; // TODO: PAL
    } else {
        ee_clock_scale = u16(1 << (4 * clock_mode + 1)); // 0 => bus clock; 1 => bus clock / 16; 2 => bus clock / 256.
    }
    gate_enable = data & 4;
    gate_type = static_cast<GateType>(data >> 3 & 1);
    gate_mode = static_cast<GateMode>(data >> 4 & 3);
    clear_counter_on_compare_match = data & 0x40;
    enabled = data & 0x80;
    compare_int_enable = data & 0x100;
    overflow_int_enable = data & 0x200;
    compare_int_flag &= !(data & 0x400);
    overflow_int_flag &= !(data & 0x800);
    counter_rem = 0;
    mode_raw = data & 0xF3FF | compare_int_flag << 10 | overflow_int_flag << 11;
    handle_events();
}

void init()
{
    for (Timer& t : timers) {
        t.reset();
    }
}

void on_hblank(bool active)
{
    for (Timer& t : timers) {
        t.update_gating(GateType::HBlank, active);
    }
}

void on_vblank(bool active)
{
    for (Timer& t : timers) {
        t.update_gating(GateType::VBlank, active);
    }
}

u32 read_io(u32 addr)
{
    Timer& timer = timers[addr >> 11 & 3];
    switch (addr & ~0x1800) {
    case Addr::TN_COUNT: return timer.read_counter(); // 10000000h + N*800h
    case Addr::TN_MODE: return timer.mode_raw; // 10000010h + N*800h
    case Addr::TN_COMP: return timer.compare; // 10000020h + N*800h
    case Addr::TN_HOLD: return timer.sbus_int_counter; // 10000030h + N*800h
    default: message::fatal(std::format("EE: Tried to read from unknown IO address {:X}", addr)); return {};
    }
}

void write_io(u32 addr, u32 data)
{
    Timer& timer = timers[addr >> 11 & 3];
    switch (addr & ~0x1800) {
    case Addr::TN_COUNT: timer.set_counter(data); break; // 10000000h + N*800h
    case Addr::TN_MODE: timer.write_to_mode(data); break; // 10000010h + N*800h
    case Addr::TN_COMP: timer.set_compare(data); break; // 10000020h + N*800h
    case Addr::TN_HOLD: timer.set_sbus_counter(data); break; // 10000030h + N*800h
    default: message::fatal(std::format("EE: Tried to write to unknown IO address {:X}", addr));
    }
}

} // namespace ee::timers
