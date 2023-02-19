#include "log.hpp"
#include "util.hpp"

#include <format>
#include <fstream>
#include <iostream>
#include <string>

namespace nanostation::log {

static std::ofstream file_log;
static std::string prev_file_output;
static u64 file_output_repeat_counter;

void dma(std::string_view output)
{
    file_out(std::format("EE INIT DMA; {}", output));
}

void ee_exception(std::string_view exception)
{
    file_out(std::format("EE EXCEPTION; {}", exception));
}

void ee_instruction(u32 paddr, std::string_view instr_output)
{
    file_out(std::format("EE; ${:08X}  {}", paddr, instr_output));
}

void ee_read(u32 paddr, auto value)
{
    file_out(std::format("EE READ; ${:0X} from ${:08X}", to_unsigned(value), paddr));
}

void ee_write(u32 paddr, auto value)
{
    file_out(std::format("EE WRITE; ${:0X} to ${:08X}", to_unsigned(value), paddr));
}

void error(std::string_view output)
{
    std::string shown_output = std::format("[ERROR] {}", output);
    std_out(shown_output);
    file_out(shown_output);
}

// TODO: std::source_location not working with clang-cl for some obscure reason
void fatal(std::string_view output /*, std::source_location loc*/)
{
    // std::string shown_output = std::format("[FATAL] {}({}:{}), function {}: {}",
    //   loc.file_name(),
    //   loc.line(),
    //   loc.column(),
    //   loc.function_name(),
    //   output);
    std::string shown_output = std::format("[FATAL] {}", output);
    std_out(shown_output);
    file_out(shown_output);
}

void file_out(std::string_view output)
{
    if constexpr (enable_file_logging) {
        if (!file_log.is_open()) {
            return;
        }
        if (output == prev_file_output) {
            ++file_output_repeat_counter;
        } else {
            if (file_output_repeat_counter > 0) {
                file_log << "<<< Repeated " << file_output_repeat_counter << " time(s). >>>\n";
                file_output_repeat_counter = 0;
            }
            prev_file_output = output;
            file_log << output << '\n';
        }
    }
}

void info(std::string_view output)
{
    std::string shown_output = std::format("[INFO] {}", output);
    std_out(shown_output);
    file_out(shown_output);
}

bool init_file()
{
    if constexpr (enable_file_logging) {
        file_log.open(log_path.data());
        return file_log.is_open();
    } else {
        return true;
    }
}

void iop_exception(std::string_view exception)
{
    file_out(std::format("IOP EXCEPTION; {}", exception));
}

void iop_instruction(u32 paddr, std::string_view instr_output)
{
    file_out(std::format("IOP; ${:08X}  {}", paddr, instr_output));
}

void iop_read(u32 paddr, auto value)
{
    file_out(std::format("IOP READ; ${:0X} from ${:08X}", to_unsigned(value), paddr));
}

void iop_write(u32 paddr, auto value)
{
    file_out(std::format("IOP WRITE; ${:0X} to ${:08X}", to_unsigned(value), paddr));
}

void std_out(std::string_view output)
{
    std::cout << output << '\n';
}

void warn(std::string_view output)
{
    std::string shown_output = std::format("[WARN] {}", output);
    std_out(shown_output);
    file_out(shown_output);
}

} // namespace nanostation::log
