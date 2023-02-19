#include "message.hpp"
#include "log.hpp"

#include "SDL.h"

#include <format>
#include <iostream>
#include <string>

namespace nanostation::message {

static SDL_Window* sdl_window; /* Must be set via 'Init' before any messages are shown. */

bool init(SDL_Window* sdl_window_param)
{
    if (sdl_window_param) {
        sdl_window = sdl_window_param;
        return true;
    } else {
        log::error("nullptr given as argument to Message::SetWindow");
        return false;
    }
}

void error(std::string_view message)
{
    log::error(message);
    if (sdl_window) {
        std::string shown_message = std::format("Error: {}", message);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", shown_message.c_str(), sdl_window);
    }
}

// TODO: std::source_location not working with clang-cl for some obscure reason
void fatal(std::string_view message /*, std::source_location loc*/)
{
    log::fatal(message /*, loc*/);
    if (sdl_window) {
        // std::string shown_message = std::format("Fatal error at {}({}:{}), function {}: {}",
        //   loc.file_name(),
        //   loc.line(),
        //   loc.column(),
        //   loc.function_name(),
        //   message);
        std::string shown_message = std::format("Fatal: {}", message);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal", shown_message.c_str(), sdl_window);
    }
}

void info(std::string_view message)
{
    log::info(message);
    if (sdl_window) {
        std::string shown_message = std::format("Info: {}", message);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", shown_message.c_str(), sdl_window);
    }
}

void warn(std::string_view message)
{
    log::warn(message);
    if (sdl_window) {
        std::string shown_message = std::format("Warning: {}", message);
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", shown_message.c_str(), sdl_window);
    }
}

} // namespace nanostation::message
