#include "cop1.hpp"

namespace ee {

using enum mips::CpuImpl;

template<> void abs_s<Interpreter>(u32 fd, u32 fs)
{
}

template<> void add_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void adda_s<Interpreter>(u32 fs, u32 ft)
{
}

template<> void bc1f<Interpreter>(s16 imm)
{
}

template<> void bc1fl<Interpreter>(s16 imm)
{
}

template<> void bc1t<Interpreter>(s16 imm)
{
}

template<> void bc1tl<Interpreter>(s16 imm)
{
}

template<> void c_eq<Interpreter>(u32 fs, u32 ft)
{
}

template<> void c_f<Interpreter>(u32 fs, u32 ft)
{
}

template<> void c_le<Interpreter>(u32 fs, u32 ft)
{
}

template<> void c_lt<Interpreter>(u32 fs, u32 ft)
{
}

template<> void cfc1<Interpreter>(u32 fs, u32 rt)
{
}

template<> void ctc1<Interpreter>(u32 fs, u32 rt)
{
}

template<> void cvt_s<Interpreter>(u32 fd, u32 fs)
{
}

template<> void cvt_w<Interpreter>(u32 fd, u32 fs)
{
}

template<> void div_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void lwc1<Interpreter>(u32 ft, u32 base, u32 imm)
{
}

template<> void madd_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void madda_s<Interpreter>(u32 fs, u32 ft)
{
}

template<> void max_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void mfc1<Interpreter>(u32 fs, u32 rt)
{
}

template<> void min_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void mov_s<Interpreter>(u32 fd, u32 fs)
{
}

template<> void msub_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void msuba_s<Interpreter>(u32 fs, u32 ft)
{
}

template<> void mtc1<Interpreter>(u32 fs, u32 rt)
{
}

template<> void mul_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void mula_s<Interpreter>(u32 fs, u32 ft)
{
}

template<> void neg_s<Interpreter>(u32 fd, u32 fs)
{
}

template<> void rsqrt_s<Interpreter>(u32 fd, u32 fs)
{
}

template<> void sqrt_s<Interpreter>(u32 fd, u32 fs)
{
}

template<> void sub_s<Interpreter>(u32 fd, u32 fs, u32 ft)
{
}

template<> void suba_s<Interpreter>(u32 fs, u32 ft)
{
}

template<> void swc1<Interpreter>(u32 ft, u32 base, u32 imm)
{
}

} // namespace ee
