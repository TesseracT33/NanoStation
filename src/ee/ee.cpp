#include "ee.hpp"

#include <cstring>
#include <utility>

namespace ee {
s32 pc;
s64 lo, lo1, hi, hi1;
GPR gpr;
} // namespace ee

template<std::integral Int> Int ee::GPR::get(int index) const
{
    Int ret;
    memcpy(&ret, &gpr[index], sizeof(Int));
    return ret;
}

u128 ee::GPR::get(Reg reg) const
{
    return gpr[std::to_underlying(reg)];
}

template<std::integral Int> void ee::GPR::set(int index, Int data)
{
}

void ee::GPR::set(Reg reg, u128 data)
{
}

template u8 ee::GPR::get<u8>(int) const;
template s8 ee::GPR::get<s8>(int) const;
template u16 ee::GPR::get<u16>(int) const;
template s16 ee::GPR::get<s16>(int) const;
template u32 ee::GPR::get<u32>(int) const;
template s32 ee::GPR::get<s32>(int) const;
template u64 ee::GPR::get<u64>(int) const;
template s64 ee::GPR::get<s64>(int) const;

template void ee::GPR::set<u8>(int, u8);
template void ee::GPR::set<s8>(int, s8);
template void ee::GPR::set<u16>(int, u16);
template void ee::GPR::set<s16>(int, s16);
template void ee::GPR::set<u32>(int, u32);
template void ee::GPR::set<s32>(int, s32);
template void ee::GPR::set<u64>(int, u64);
template void ee::GPR::set<s64>(int, s64);
