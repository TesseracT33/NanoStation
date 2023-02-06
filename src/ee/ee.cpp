#include "ee.hpp"
#include "cop0.hpp"
#include "exceptions.hpp"
#include "mips/disassembler.hpp"
#include "mips/mips.hpp"
#include "mmu.hpp"
#include "scheduler.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <optional>

namespace ee {
bool in_branch_delay_slot;
u32 jump_addr, pc;
u32 sa;
Reg128 lo, hi;
mips::Gpr<Reg128> gpr;

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
    u32 instr = virtual_read<u32, ee::Alignment::Aligned, ee::MemOp::InstrFetch>(pc);
    pc += 4;
    mips::disassemble_ee<mips::CpuImpl::Interpreter>(instr);
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
    static constexpr size_t bios_size = 4 * 1024 * 1024;
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

} // namespace ee
