#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

namespace ee {
template<mips::CpuImpl> void abs_s(u32 fd, u32 fs);
template<mips::CpuImpl> void add_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void adda_s(u32 fs, u32 ft);
template<mips::CpuImpl> void bc1f(s16 imm);
template<mips::CpuImpl> void bc1fl(s16 imm);
template<mips::CpuImpl> void bc1t(s16 imm);
template<mips::CpuImpl> void bc1tl(s16 imm);
template<mips::CpuImpl> void c_eq(u32 fs, u32 ft);
template<mips::CpuImpl> void c_f(u32 fs, u32 ft);
template<mips::CpuImpl> void c_le(u32 fs, u32 ft);
template<mips::CpuImpl> void c_lt(u32 fs, u32 ft);
template<mips::CpuImpl> void cfc1(u32 fs, u32 rt);
template<mips::CpuImpl> void ctc1(u32 fs, u32 rt);
template<mips::CpuImpl> void cvt_s(u32 fd, u32 fs);
template<mips::CpuImpl> void cvt_w(u32 fd, u32 fs);
template<mips::CpuImpl> void div_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void lwc1(u32 ft, u32 base, u32 imm);
template<mips::CpuImpl> void madd_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void madda_s(u32 fs, u32 ft);
template<mips::CpuImpl> void max_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void mfc1(u32 fs, u32 rt);
template<mips::CpuImpl> void min_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void mov_s(u32 fd, u32 fs);
template<mips::CpuImpl> void msub_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void msuba_s(u32 fs, u32 ft);
template<mips::CpuImpl> void mtc1(u32 fs, u32 rt);
template<mips::CpuImpl> void mul_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void mula_s(u32 fs, u32 ft);
template<mips::CpuImpl> void neg_s(u32 fd, u32 fs);
template<mips::CpuImpl> void rsqrt_s(u32 fd, u32 fs);
template<mips::CpuImpl> void sqrt_s(u32 fd, u32 fs);
template<mips::CpuImpl> void sub_s(u32 fd, u32 fs, u32 ft);
template<mips::CpuImpl> void suba_s(u32 fs, u32 ft);
template<mips::CpuImpl> void swc1(u32 ft, u32 base, u32 imm);
} // namespace ee
