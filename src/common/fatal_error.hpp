#pragma once

#include <print>
#include <source_location>

template<typename... Args>
void fatal_error(std::format_string<Args...> format,
  Args&&... args,
  std::source_location location = std::source_location::current())
{
    std::print("[FATAL] At {}({}:{}) '{}': ",
      location.file_name(),
      location.line(),
      location.column(),
      location.function_name());
    std::println(format, std::forward<Args>(args)...);
}
