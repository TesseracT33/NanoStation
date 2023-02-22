#include "timers.hpp"
#include "ee.hpp"
#include "frontend/message.hpp"
#include "scheduler.hpp"

#include <array>
#include <cassert>
#include <format>
#include <utility>

namespace ee::timers {

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
    u64 time_last_counter_refresh;
    Interrupt intc_interrupt;
    scheduler::EventType interrupt_event;
    u16 compare;
    u16 counter;
    u16 ee_clock_scale;
    u16 mode_raw;
    u16 sbus_int_counter;
    GateMode gate_mode;
    GateType gate_type;
    u8 index;
    bool clear_counter_on_compare_match;
    bool compare_int_flag;
    bool compare_int_enable;
    bool enabled;
    bool gated;
    bool gate_enable;
    bool overflow_int_flag;
    bool overflow_int_enable;

    void deschedule_event();
    bool is_counting() const;
    void on_compare_match();
    void on_hold_match();
    void on_overflow();
    u16 refresh_and_read_counter();
    template<bool check_overflow = true, bool check_compare = true> void schedule_event();
    void schedule_event_no_compare();
    void schedule_event_no_overflow();
    void set_compare(u32 value);
    void set_counter(u32 value);
    void set_sbus_counter(u32 value);
    void write_to_mode(u32 data);
};

static std::array<Timer, 4> timers;

static void update_gating(GateType gate_type, bool active);

void Timer::deschedule_event()
{
    scheduler::remove_event(interrupt_event);
}

bool Timer::is_counting() const
{
    return enabled && !gated;
}

void Timer::on_compare_match()
{
    if (compare_int_enable) {
        if (clear_counter_on_compare_match) {
            counter = 0;
            time_last_counter_refresh = get_time();
        }
        if (!std::exchange(compare_int_flag, 1)) {
            raise_intc(intc_interrupt);
        }
        schedule_event_no_compare();
    }
}

void Timer::on_hold_match()
{
    assert(index < 2);
    raise_intc(Interrupt::SBUS);
    schedule_event();
}

void Timer::on_overflow()
{
    if (overflow_int_enable && !std::exchange(overflow_int_flag, 1)) {
        raise_intc(intc_interrupt);
        schedule_event_no_overflow();
    }
}

u16 Timer::refresh_and_read_counter()
{
    u64 global_time = get_time();
    if (is_counting()) {
        u64 delta = global_time - time_last_counter_refresh;
        counter += u16(delta / ee_clock_scale); // TODO: incorporate remainder?
    }
    time_last_counter_refresh = global_time;
    return counter;
}

template<bool check_overflow, bool check_compare> void Timer::schedule_event()
{
    if (gated) return;
    u64 fewest_ticks = -1;
    scheduler::EventCallback handler{};
    if (check_overflow && overflow_int_enable && !overflow_int_flag) {
        u64 time_until_overflow = 0x10000 - counter;
        if (time_until_overflow < fewest_ticks) {
            handler = {}; // TODO
        }
    }
    if (check_compare && compare_int_enable && !compare_int_flag) {
        u64 time_until_match = compare - counter;
        if (compare < counter) {
            time_until_match += 0x10000;
        }
        if (time_until_match < fewest_ticks) {
            handler = {};
        }
    }
    if (index < 2) {
        u64 time_until_sbus = sbus_int_counter - counter;
        if (sbus_int_counter < counter) {
            time_until_sbus += 0x10000;
        }
        if (time_until_sbus < fewest_ticks) {
            handler = {};
        }
    }
    if (handler) {
        scheduler::add_event_or_change_time(interrupt_event, fewest_ticks * ee_clock_scale, handler);
    }
}

void Timer::schedule_event_no_compare()
{
    schedule_event<true, false>();
}

void Timer::schedule_event_no_overflow()
{
    schedule_event<false, true>();
}

void Timer::set_compare(u32 value)
{
    compare = value & 0xFFFF;
    if (enabled) {
        if (refresh_and_read_counter() == compare) {
            on_compare_match();
        } else if (compare_int_enable && !compare_int_flag) {
            schedule_event();
        }
    }
}

void Timer::set_counter(u32 value)
{
    time_last_counter_refresh = get_time();
    counter = value & 0xFFFF;
    if (enabled) {
        if (counter == compare) {
            on_compare_match();
        } else {
            schedule_event();
        }
    }
}

void Timer::set_sbus_counter(u32 value)
{
    assert(index < 2);
    sbus_int_counter = value & 0xFFFF;
    if (enabled) {
        if (refresh_and_read_counter() == sbus_int_counter) {
            on_hold_match();
        } else {
            schedule_event();
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
    is_counting() ? schedule_event() : deschedule_event();
}

void init()
{
    timers = {};
    for (int i = 0; i < 4; ++i) {
        timers[i].index = u8(i);
        timers[i].intc_interrupt = static_cast<Interrupt>(std::to_underlying(Interrupt::Timer0) + i);
        timers[i].interrupt_event =
          static_cast<scheduler::EventType>(std::to_underlying(scheduler::EventType::EETimer0Interrupt) + i);
    }
}

void on_hblank(bool active)
{
    update_gating(GateType::HBlank, active);
}

void on_vblank(bool active)
{
    update_gating(GateType::VBlank, active);
}

u32 read(u32 addr)
{
    auto timer = [addr]() -> Timer& { return timers[addr >> 11]; };
    switch (addr & ~0x1800) {
    case 0x1000'0000: return timer().refresh_and_read_counter(); // 10000000h + N*800h TN_COUNT
    case 0x1000'0010: return timer().mode_raw; // 10000010h + N*800h TN_MODE
    case 0x1000'0020: return timer().compare; // 10000020h + N*800h TN_COMP
    case 0x1000'0030: return timer().sbus_int_counter; // 10000030h + N*800h TN_HOLD
    default: message::fatal(std::format("Tried to read from unknown IO address {:X}", addr)); return {};
    }
}

void update_gating(GateType gate_type, bool active)
{
    for (Timer& t : timers) {
        if (t.gate_enable && t.gate_type == gate_type) {
            using enum GateMode;
            switch (t.gate_mode) {
            case CountWhileNotGated: t.gated = active; break;
            case ResetCountOnGateHi:
                t.gated = false;
                if (active) t.set_counter(0);
                break;
            case ResetCountOnGateLo:
                t.gated = false;
                if (!active) t.set_counter(0);
                break;
            case ResetCountOnGateTrans:
                t.gated = false;
                t.set_counter(0);
                break;
            }
            // TODO: update events
        }
    }
}

void write(u32 addr, u32 data)
{
    auto timer = [addr]() -> Timer& { return timers[addr >> 11]; };
    switch (addr & ~0x1800) {
    case 0x1000'0000: timer().set_counter(data); break; // 10000000h + N*800h TN_COUNT
    case 0x1000'0010: timer().write_to_mode(data); break; // 10000010h + N*800h TN_MODE
    case 0x1000'0020: timer().set_compare(data); break; // 10000020h + N*800h TN_COMP
    case 0x1000'0030: timer().set_sbus_counter(data); break; // 10000030h + N*800h TN_HOLD
    default: message::fatal(std::format("Tried to write to unknown IO address {:X}", addr));
    }
}

} // namespace ee::timers
