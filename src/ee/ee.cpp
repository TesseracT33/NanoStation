#include "ee.hpp"
#include "cop0.hpp"
#include "exceptions.hpp"
#include "frontend/message.hpp"
#include "mips/disassembler.hpp"
#include "mips/mips.hpp"
#include "mmu.hpp"
#include "scheduler.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <string>

namespace ee {

static u32 cycle_counter;

static void fetch_decode_exec();

void add_initial_events()
{
    reload_count_compare_event<true>();
}

void advance_pipeline(u32 cycles)
{
    cycle_counter += cycles;
    cop0.count += cycles;
    cycles_since_updated_random += cycles;
}

void check_interrupts()
{
}

void fetch_decode_exec()
{
    u32 instr = virtual_read<u32, Alignment::Aligned, MemOp::InstrFetch>(pc);
    pc += 4;
    mips::disassemble_ee<mips::CpuImpl::Interpreter>(instr);
    advance_pipeline(1);
}

bool init()
{
    std::memset(&gpr, 0, sizeof(gpr));
    std::memset(&lo, 0, sizeof(lo));
    std::memset(&hi, 0, sizeof(hi));
    jump_addr = pc = 0;
    in_branch_delay_slot = false;

    reset_exception();

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
        nanostation::message::error(std::string("Failed to load bios; ") + expected_bios.error());
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

} // namespace ee
