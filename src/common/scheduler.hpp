#pragma once

#include <thread>

#include "inplace_function.hpp"
#include "numtypes.hpp"

namespace scheduler {

using EventCallback = InplaceFunction<void()>;

enum class EventType : u8 {
    EECopCountCompareMatch,
    EETimer0Match,
    EETimer0Overflow,
    EETimer0Sbus,
    EETimer1Match,
    EETimer1Overflow,
    EETimer1Sbus,
    EETimer2Match,
    EETimer2Overflow,
    EETimer3Match,
    EETimer3Overflow,
};

void add_event(EventType event, s64 ee_cycles_until_fire, EventCallback callback);
void add_event_or_change_time(EventType event_type, s64 ee_cycles_until_fire, EventCallback callback);
void change_event_time(EventType event, s64 ee_cycles_until_fire);
void init();
void remove_event(EventType event);
void run(std::stop_token stop_token);

} // namespace scheduler
