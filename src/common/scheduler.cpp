#include "scheduler.hpp"
#include "ee/ee.hpp"
#include "iop/iop.hpp"

#include <vector>

namespace scheduler {

struct Event {
    s64 ee_cycles_until_fire; /* signed so that we can subtract a duration and check if the result is negative */
    EventCallback callback;
    EventType event_type;
};

static void check_events(s64 ee_cycles_elapsed);

static std::vector<Event> events; /* sorted by when they will occur */

void add_event(EventType event_type, s64 ee_cycles_until_fire, EventCallback callback)
{
    /* (From N64 code) Compensate for the fact that we may be in the middle of a CPU update, and times for other events
        have not updated yet. TODO: We are assuming that only the main CPU can cause an event to be added.
        Is it ok? */
    s64 elapsed_cycles_since_step_start = 0;
    s64 enqueue_time = ee_cycles_until_fire + elapsed_cycles_since_step_start;
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (enqueue_time < it->ee_cycles_until_fire) {
            events.emplace(it, enqueue_time, callback, event_type);
            return;
        }
    }
    events.emplace_back(enqueue_time, callback, event_type);
}

void add_event_or_change_time(EventType event_type, s64 ee_cycles_until_fire, EventCallback callback)
{
    s64 elapsed_cycles_since_step_start = 0;
    s64 enqueue_time = ee_cycles_until_fire + elapsed_cycles_since_step_start;
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (it->event_type == event_type) {
            EventCallback callback = it->callback;
            events.erase(it);
            add_event(event_type, enqueue_time, callback);
            return;
        }
    }
    add_event(event_type, enqueue_time, callback);
}

void change_event_time(EventType event_type, s64 ee_cycles_until_fire)
{
    s64 elapsed_cycles_since_step_start = 0;
    s64 enqueue_time = ee_cycles_until_fire + elapsed_cycles_since_step_start;
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (it->event_type == event_type) {
            it->ee_cycles_until_fire = enqueue_time;
            EventCallback callback = it->callback;
            events.erase(it);
            add_event(event_type, enqueue_time, callback);
            return;
        }
    }
}

void check_events(s64 ee_cycles_elapsed)
{
    for (auto it = events.begin(); it != events.end();) {
        it->ee_cycles_until_fire -= ee_cycles_elapsed;
        if (it->ee_cycles_until_fire <= 0) {
            /* erase element before invoking callback, in case it mutates the event list */
            EventCallback callback = it->callback;
            events.erase(it);
            callback();
            it = events.begin();
        } else {
            ++it;
        }
    }
}

void init()
{
    events.clear();
    events.reserve(16);
    ee::add_initial_events();
}

void remove_event(EventType event_type)
{
    for (auto it = events.begin(); it != events.end(); ++it) {
        if (it->event_type == event_type) {
            events.erase(it);
            return;
        }
    }
}

void run(std::stop_token stop_token)
{
    init();

    // EE 294.912 MHz, IOP 36.864 Mhz in ps2 mode. Ratio: 8
    constexpr u32 ee_cycles_per_update = 1024;
    u32 ee_cycle_overrun = 0;
    // u32 iop_cycle_overrun = 0;
    while (!stop_token.stop_requested()) {
        if (ee_cycle_overrun < ee_cycles_per_update) {
            u32 ee_step = ee_cycles_per_update - ee_cycle_overrun;
            u32 ee_actual_step = ee::run(ee_step);
            ee_cycle_overrun = ee_actual_step - ee_step;
            check_events(ee_actual_step);
            // u32 iop_step = ee_actual_step / 8;
            //  u32 iop_actual_step = iop::run(iop_step);
        } else [[unlikely]] {
            ee_cycle_overrun -= ee_cycles_per_update;
        }
    }
}

} // namespace scheduler
