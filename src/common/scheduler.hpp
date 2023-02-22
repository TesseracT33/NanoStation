#pragma once

#include "types.hpp"

namespace scheduler {

using EventCallback = void (*)();

enum class EventType {
    EECopCountCompareMatch,
    EETimer0Interrupt,
    EETimer1Interrupt,
    EETimer2Interrupt,
    EETimer3Interrupt
};

void add_event(EventType event, s64 cpu_cycles_until_fire, EventCallback callback);
void add_event_or_change_time(EventType event_type, s64 cpu_cycles_until_fire, EventCallback callback);
void change_event_time(EventType event, s64 cpu_cycles_until_fire);
void init();
void remove_event(EventType event);
void run();
void stop();

} // namespace scheduler
