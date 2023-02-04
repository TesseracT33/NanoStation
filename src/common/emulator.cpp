#include "emulator.hpp"
#include "ee/ee.hpp"
#include "scheduler.hpp"

namespace emulator {
static bool bios_loaded;
static bool game_loaded;
static bool running;

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
    return false;
}

bool load_game(std::filesystem::path const& game_path)
{
    return false;
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
    scheduler::run();
}

void stop()
{
    scheduler::stop();
}

} // namespace emulator
