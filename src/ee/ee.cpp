#include "ee.hpp"

namespace ee {
bool in_branch_delay_slot;
u32 jump_addr, pc;
u32 sa;
Reg128 lo, hi;
GPR gpr;

static bool jump_is_pending;

void cancel_jump()
{
    in_branch_delay_slot = jump_is_pending = false;
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

void prepare_jump(u32 target)
{
    in_branch_delay_slot = jump_is_pending = true;
}

} // namespace ee
