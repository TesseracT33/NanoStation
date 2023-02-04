#include "scheduler.hpp"
#include "ee/ee.hpp"

#include <vector>

namespace scheduler {

struct Event {
    EventType event_type;
    s64 cpu_cycles_until_fire; /* signed so that we can subtract a duration and check if the result is negative */
    EventCallback callback;
};

static void check_events(s64 cpu_cycle_step);

static bool quit;
static std::vector<Event> events; /* sorted by when they will occur */

void add_event(EventType event_type, s64 cpu_cycles_until_fire, EventCallback callback)
{
    /* Compensate for the fact that we may be in the middle of a CPU update, and times for other events
        have not updated yet. TODO: We are assuming that only the main CPU can cause an event to be added.
        Is it ok? */
    s64 elapsed_cycles_since_step_start = 0;
    s64 enqueue_time = cpu_cycles_until_fire + elapsed_cycles_since_step_start;
    for (auto it = events.cbegin(); it != events.cend(); ++it) {
        if (enqueue_time < it->cpu_cycles_until_fire) {
            events.insert(it, Event{ event_type, enqueue_time, callback });
            return;
        }
    }
    events.push_back(Event{ event_type, enqueue_time, callback });
}

void change_event_time(EventType event_type, s64 cpu_cycles_until_fire)
{
    s64 elapsed_cycles_since_step_start = 0;
    s64 enqueue_time = cpu_cycles_until_fire + elapsed_cycles_since_step_start;
    for (auto it = events.cbegin(); it != events.cend(); ++it) {
        if (it->event_type == event_type) {
            EventCallback callback = it->callback;
            events.erase(it);
            add_event(event_type, enqueue_time, callback);
            return;
        }
    }
}

void check_events(s64 cpu_cycle_step)
{
    for (auto it = events.begin(); it != events.end();) {
        it->cpu_cycles_until_fire -= cpu_cycle_step;
        if (it->cpu_cycles_until_fire <= 0) {
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
    quit = false;
    events.clear();
    events.reserve(16);
    ee::add_initial_events();
}

void remove_event(EventType event_type)
{
    for (auto it = events.cbegin(); it != events.cend(); ++it) {
        if (it->event_type == event_type) {
            events.erase(it);
            return;
        }
    }
}

void run()
{
    init();

    static constexpr s64 ee_cycles_per_update = 1024;
    s64 ee_cycle_overrun = 0;
    while (!quit) {
        s64 ee_step_dur = ee_cycles_per_update - ee_cycle_overrun;
        ee_cycle_overrun = ee::run(ee_step_dur);
        check_events(ee_step_dur);
    }
}

void stop()
{
    quit = true;
}

} // namespace scheduler
