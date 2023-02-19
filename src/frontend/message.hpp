#pragma once

#include <source_location>
#include <string_view>

struct SDL_Window;

namespace nanostation::message {

void error(std::string_view message);
void fatal(std::string_view message /*, std::source_location loc = std::source_location::current()*/);
void info(std::string_view message);
bool init(SDL_Window* sdl_window);
void warn(std::string_view message);

} // namespace nanostation::message
