#include "emulator.hpp"
#include "ee/ee.hpp"
#include "scheduler.hpp"

#include <thread>

namespace emulator {

static std::jthread emu_thread;
// static bool bios_loaded;
// static bool game_loaded;
// static bool running;

bool init()
{
    if (!ee::init()) {
        return false;
    }
    scheduler::init();
    return true;
}

bool load_bios(std::filesystem::path const& bios_path)
{
    return ee::load_bios(bios_path);
}

bool load_game(std::filesystem::path const& game_path)
{
    (void)game_path;
    return true;
}

void pause()
{
}

void reset()
{
}

void resume()
{
}

void run()
{
    emu_thread = std::jthread([](std::stop_token stop_token) { scheduler::run(stop_token); });
    emu_thread.detach();
}

void stop()
{
    emu_thread = {};
}

} // namespace emulator
