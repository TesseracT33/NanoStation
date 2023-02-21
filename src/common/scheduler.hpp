#pragma once

#include "types.hpp"

namespace scheduler {

using EventCallback = void (*)();

enum class EventType {
    EECopCountCompareMatch,
    EETimer0CounterCompareMatch,
    EETimer1CounterCompareMatch,
    EETimer2CounterCompareMatch,
    EETimer3CounterCompareMatch,
};

void add_event(EventType event, s64 cpu_cycles_until_fire, EventCallback callback);
void change_event_time(EventType event, s64 cpu_cycles_until_fire);
void init();
void remove_event(EventType event);
void run();
void stop();

} // namespace scheduler
