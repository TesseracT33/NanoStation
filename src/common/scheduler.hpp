#pragma once

#include <thread>

#include "numtypes.hpp"

namespace scheduler {

using EventCallback = void (*)();

enum class EventType : u8 {
    EECopCountCompareMatch,
    EETimer0Interrupt,
    EETimer1Interrupt,
    EETimer2Interrupt,
    EETimer3Interrupt,
};

void add_event(EventType event, s64 ee_cycles_until_fire, EventCallback callback);
void add_event_or_change_time(EventType event_type, s64 ee_cycles_until_fire, EventCallback callback);
void change_event_time(EventType event, s64 ee_cycles_until_fire);
void init();
void remove_event(EventType event);
void run(std::stop_token stop_token);

} // namespace scheduler
