#include "iop.hpp"
#include "exceptions.hpp"
#include "memory.hpp"
#include "mips/disassembler.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>

namespace iop {

static u32 cycle_counter;
static u64 global_time_last_step_begin;

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

u64 get_global_time()
{
    return global_time_last_step_begin + cycle_counter;
}

void fetch_decode_exec()
{
    u32 instr = read<u32, Alignment::Aligned, MemOp::InstrFetch>(pc);
    pc += 4;
    mips::disassemble_iop<mips::CpuImpl::Interpreter>(instr);
    advance_pipeline(1);
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
    std::expected<std::vector<u8>, std::string> expected_bios = read_file(path, bios_size);
    if (expected_bios) {
        std::vector<u8> const& bios_val = expected_bios.value();
        std::copy(bios_val.cbegin(), bios_val.cend(), bios.begin());
        return true;
    } else {
        // TODO: user message
        return false;
    }
}

u32 run(u32 cycles)
{
    cycle_counter = 0;
    while (cycle_counter < cycles) {
        fetch_decode_exec();
    }
    global_time_last_step_begin += cycle_counter;
    return cycle_counter - cycles;
}

} // namespace iop
