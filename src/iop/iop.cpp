#include "iop.hpp"
#include "exceptions.hpp"
#include "memory.hpp"
#include "mips/disassembler.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <optional>

namespace iop {

static u32 cycle_counter;

static void fetch_decode_exec();

void add_initial_events()
{
}

void advance_pipeline(u32 cycles)
{
    cycle_counter += cycles;
}

void check_interrupts()
{
}

void fetch_decode_exec()
{
    u32 instr = read<u32, Alignment::Aligned, MemOp::InstrFetch>(pc);
    pc += 4;
    mips::disassemble_iop<mips::CpuImpl::Interpreter>(instr);
}

bool init()
{
    std::memset(&gpr, 0, sizeof(gpr));
    std::memset(&lo, 0, sizeof(lo));
    std::memset(&hi, 0, sizeof(hi));
    jump_addr = pc = 0;
    in_branch_delay_slot = false;

    return true;
}

void jump(u32 target)
{
    assert(!in_branch_delay_slot); // TODO: going beyond this can result in stack overflow. First need to know what the
                                   // behaviour is
    in_branch_delay_slot = true;
    fetch_decode_exec();
    in_branch_delay_slot = false;
    if (!exception_occurred) pc = target;
}

bool load_bios(std::filesystem::path const& path)
{
    std::optional<std::array<u8, bios_size>> opt_bios = read_file_into_array<bios_size>(path);
    if (opt_bios) {
        std::array<u8, bios_size> const& bios_val = opt_bios.value();
        std::copy(bios_val.cbegin(), bios_val.cend(), bios.begin());
        return true;
    } else {
        return false;
    }
}

u32 run(u32 cycles)
{
    cycle_counter = 0;
    while (cycle_counter < cycles) {
        fetch_decode_exec();
    }
    return cycle_counter - cycles;
}

} // namespace iop
