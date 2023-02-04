#include "ee.hpp"
#include "cop0.hpp"
#include "disassembler.hpp"
#include "exceptions.hpp"
#include "mmu.hpp"

#include <cassert>

namespace ee {
bool in_branch_delay_slot;
u32 jump_addr, pc;
u32 sa;
Reg128 lo, hi;
GPR gpr;

static u32 cycle_counter;

static void fetch_decode_exec();

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
    u32 instr = virtual_read<4, ee::Alignment::Aligned, ee::MemOp::InstrFetch>(pc);
    pc += 4;
    interpreter::disassemble(instr);
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

u32 run(u32 cycles)
{
    cycle_counter = 0;
    while (cycle_counter < cycles) {
        fetch_decode_exec();
    }
    return cycle_counter - cycles;
}

} // namespace ee
