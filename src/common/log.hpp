#pragma once

#include <print>

template<typename... Args> void log_error(std::format_string<Args...> format, Args&&... args)
{
    std::print("[ERROR] ");
    std::println(format, std::forward<Args>(args)...);
}

template<typename... Args> void log_fatal(std::format_string<Args...> format, Args&&... args)
{
    std::print("[FATAL] ");
    std::println(format, std::forward<Args>(args)...);
}

template<typename... Args> void log_info(std::format_string<Args...> format, Args&&... args)
{
    std::print("[INFO] ");
    std::println(format, std::forward<Args>(args)...);
}

template<typename... Args> void log_warn(std::format_string<Args...> format, Args&&... args)
{
    std::print("[WARN] ");
    std::println(format, std::forward<Args>(args)...);
}
