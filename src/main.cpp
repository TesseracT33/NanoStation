#include "emulator.hpp"
#include "log.hpp"

#include <cstdlib>

int main(int argc, char* argv[])
{
    // CLI arguments (mandatory for now):
    //   1: game path
    //   2: bios path

    if (!emulator::init()) {
        log_fatal("Failed to init emulator!");
        return EXIT_FAILURE;
    }

    if (argc > 1) {
        char const* game_path = argv[1];
        if (!emulator::load_game(game_path)) {
            log_fatal("Failed to load game at path {}", game_path);
            return EXIT_FAILURE;
        }
    } else {
        return EXIT_FAILURE;
    }

    if (argc > 2) {
        char const* bios_path = argv[2];
        if (!emulator::load_bios(bios_path)) {
            log_fatal("Failed to load bios at path {}", bios_path);
            return EXIT_FAILURE;
        }
    } else {
        return EXIT_FAILURE;
    }

    emulator::run();
}
