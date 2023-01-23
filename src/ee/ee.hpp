#pragma once

#include "reg128.hpp"
#include "types.hpp"

#include <array>
#include <concepts>
#include <cstring>
#include <utility>

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
    template<std::integral Int> Int get(std::integral auto index) const;
    void set(std::integral auto index, std::integral auto data);
    void set(std::integral auto index, m128i data);
    template<uint lane> void set(std::integral auto index, std::integral auto data);

    Reg128& operator[](std::integral auto index);

private:
    std::array<Reg128, 32> gpr{};
} extern gpr;

extern bool in_branch_delay_slot;
extern u32 jump_addr, pc;
extern u64 lo, lo1, hi, hi1;

bool init();
void prepare_jump(u32 target);

} // namespace ee

template<std::integral Int> Int ee::GPR::get(std::integral auto index) const
{
    Int ret;
    std::memcpy(&ret, &gpr[index], sizeof(Int));
    return ret;
}

void ee::GPR::set(std::integral auto index, std::integral auto data)
{
    gpr[index].set(data);
}

void ee::GPR::set(std::integral auto index, m128i data)
{
    gpr[index].set(data);
}

template<uint lane> void ee::GPR::set(std::integral auto index, std::integral auto data)
{
    gpr[index].set<lane>(data);
}

Reg128& ee::GPR::operator[](std::integral auto index)
{
    return gpr[index];
}
