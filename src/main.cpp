#include "emulator.hpp"
#include "log.hpp"

#include <cstdlib>
#include <iostream>

int main(int argc, char* argv[])
{
    // CLI arguments (mandatory for now):
    //   1: game path
    //   2: bios path

    if (!nanostation::log::init_file()) {
        std::cerr << "[warning] Failed to create file log\n";
    }

    if (!emulator::init()) {
        return EXIT_FAILURE;
    }

    if (argc > 1) {
        char const* game_path = argv[1];
        if (!emulator::load_game(game_path)) {
            std::cerr << "[error] Failed to load game at path " << game_path << '\n';
            return EXIT_FAILURE;
        }
    } else {
        return EXIT_FAILURE;
    }

    if (argc > 2) {
        char const* bios_path = argv[2];
        if (!emulator::load_bios(bios_path)) {
            std::cerr << "[error] Failed to load bios at path " << bios_path << '\n';
            return EXIT_FAILURE;
        }
    } else {
        return EXIT_FAILURE;
    }

    emulator::run();
}
