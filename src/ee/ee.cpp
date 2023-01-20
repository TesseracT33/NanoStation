#include "ee.hpp"

namespace ee {
bool in_branch_delay_slot;
u32 jump_addr, pc;
u64 lo, lo1, hi, hi1;
GPR gpr;

void prepare_jump(u32 target)
{
    in_branch_delay_slot = true;
}

} // namespace ee
