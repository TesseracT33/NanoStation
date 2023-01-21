#include "emulator.hpp"

#include <cstdlib>

int main(int argc, char* argv[])
{
    if (!emulator::init()) {
        return EXIT_FAILURE;
    }
    emulator::run();
}
