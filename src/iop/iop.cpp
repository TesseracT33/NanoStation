#include "iop.hpp"

namespace iop {

GPR gpr;
bool in_branch_delay_slot;
u32 lo, hi, jump_addr, pc;

void jump(u32 target)
{
}

} // namespace iop
