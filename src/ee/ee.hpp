#pragma once

#include "types.hpp"

#include <array>
#include <concepts>

namespace ee {

enum class Reg {
    zero,
    at,
    v0,
    v1,
    a0,
    a1,
    a2,
    a3,
    t0,
    t1,
    t2,
    t3,
    t4,
    t5,
    t6,
    t7,
    s0,
    s1,
    s2,
    s3,
    s4,
    s5,
    s6,
    s7,
    t8,
    t9,
    k0,
    k1,
    gp,
    sp,
    fp,
    ra
};

struct GPR {
    template<std::integral Int> Int get(int index) const;
    u128 get(Reg reg) const;
    template<std::integral Int> void set(int index, Int data);
    void set(Reg reg, u128 data);

private:
    std::array<u128, 32> gpr{};
} extern gpr;

extern s32 pc;
extern s64 lo, lo1, hi, hi1;

} // namespace ee
