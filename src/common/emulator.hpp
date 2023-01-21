#pragma once

#include <filesystem>

namespace emulator {
bool init();
bool load_bios(std::filesystem::path const& bios_path);
bool load_game(std::filesystem::path const& game_path);
void pause();
void reset();
void resume();
void run();
void stop();
} // namespace emulator
