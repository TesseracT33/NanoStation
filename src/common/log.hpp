#pragma once

#include "build_options.hpp"
#include "numtypes.hpp"

#include <source_location>
#include <string_view>

namespace nanostation::log {

void dma(std::string_view output);
void ee_exception(std::string_view exception);
void ee_instruction(u32 paddr, std::string_view instr_output);
void ee_read(u32 paddr, auto value);
void ee_write(u32 paddr, auto value);
void error(std::string_view output);
void fatal(std::string_view output /*, std::source_location loc = std::source_location::current()*/);
void file_out(std::string_view output);
void info(std::string_view output);
bool init_file();
void iop_exception(std::string_view exception);
void iop_instruction(u32 paddr, std::string_view instr_output);
void iop_read(u32 paddr, auto value);
void iop_write(u32 paddr, auto value);
void std_out(std::string_view output);
void warn(std::string_view output);

} // namespace nanostation::log
