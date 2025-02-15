#include "message.hpp"
#include "log.hpp"

#include "SDL3/SDL.h"

#include <format>
#include <stdexcept>
#include <string>

namespace message {

static SDL_Window* sdl_window; /* Must be set via 'Init' before any messages are shown. */

void error(std::string_view message)
{
    // log_error(message);
    std::string shown_message = std::format("Error: {}", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Error", shown_message.c_str(), sdl_window);
}

// TODO: std::source_location not working with clang-cl for some obscure reason
void fatal(std::string_view message /*, std::source_location loc*/)
{
    // log_fatal(message /*, loc*/);
    //  std::string shown_message = std::format("Fatal error at {}({}:{}), function {}: {}",
    //    loc.file_name(),
    //    loc.line(),
    //    loc.column(),
    //    loc.function_name(),
    //    message);
    std::string shown_message = std::format("Fatal: {}", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Fatal", shown_message.c_str(), sdl_window);
}

void info(std::string_view message)
{
    // log_info(message);
    std::string shown_message = std::format("Info: {}", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Information", shown_message.c_str(), sdl_window);
}

void init(SDL_Window* sdl_window_param)
{
    if (!sdl_window_param) {
        throw std::invalid_argument("Null SDL_Window argument given to message::init");
    }
    sdl_window = sdl_window_param;
}

void warn(std::string_view message)
{
    // log_warn(message);
    std::string shown_message = std::format("Warning: {}", message);
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "Warning", shown_message.c_str(), sdl_window);
}

} // namespace message
