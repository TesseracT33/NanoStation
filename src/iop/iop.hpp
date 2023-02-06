#pragma once

#include "types.hpp"

#include <array>
#include <concepts>

namespace iop {



struct GPR {
    u32 get(u32 idx) const { return gpr[idx]; }
    u32 operator[](u32 idx) const { return gpr[idx]; } // return by value so that writes have to be made through 'set'
    void set(u32 idx, std::integral auto val)
        requires(sizeof(val) <= 4)
    {
        gpr[idx] = val;
        gpr[0] = 0;
    }

private:
    std::array<u32, 32> gpr{};
} extern gpr;

extern bool in_branch_delay_slot;
extern u32 lo, hi, jump_addr, pc;

void jump(u32 target);

} // namespace iop
