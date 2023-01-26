#include "ee.hpp"

namespace ee {
bool in_branch_delay_slot;
u32 jump_addr, pc;
u64 sa;
Reg128 lo, lo1, hi, hi1;
GPR gpr;

bool init()
{
    std::memset(&gpr, 0, sizeof(gpr));
    lo = lo1 = hi = hi1 = jump_addr = pc = 0;
    in_branch_delay_slot = false;

    return true;
}

void prepare_jump(u32 target)
{
    in_branch_delay_slot = true;
}

} // namespace ee
