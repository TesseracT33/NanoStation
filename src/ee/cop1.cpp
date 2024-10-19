#include "cop1.hpp"

namespace ee {

using enum mips::CpuImpl;

void abs_s(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void add_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void adda_s(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void bc1f(s16 imm)
{
    (void)imm;
}

void bc1fl(s16 imm)
{
    (void)imm;
}

void bc1t(s16 imm)
{
    (void)imm;
}

void bc1tl(s16 imm)
{
    (void)imm;
}

void c_eq(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void c_f(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void c_le(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void c_lt(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void cfc1(u32 fs, u32 rt)
{
    (void)fs;
    (void)rt;
}

void ctc1(u32 fs, u32 rt)
{
    (void)fs;
    (void)rt;
}

void cvt_s(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void cvt_w(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void div_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void lwc1(u32 ft, u32 base, u32 imm)
{
    (void)ft;
    (void)base;
    (void)imm;
}

void madd_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void madda_s(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void max_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void mfc1(u32 fs, u32 rt)
{
    (void)fs;
    (void)rt;
}

void min_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void mov_s(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void msub_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void msuba_s(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void mtc1(u32 fs, u32 rt)
{
    (void)fs;
    (void)rt;
}

void mul_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void mula_s(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void neg_s(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void rsqrt_s(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void sqrt_s(u32 fd, u32 fs)
{
    (void)fd;
    (void)fs;
}

void sub_s(u32 fd, u32 fs, u32 ft)
{
    (void)fd;
    (void)fs;
    (void)ft;
}

void suba_s(u32 fs, u32 ft)
{
    (void)fs;
    (void)ft;
}

void swc1(u32 ft, u32 base, u32 imm)
{
    (void)ft;
    (void)base;
    (void)imm;
}

} // namespace ee
