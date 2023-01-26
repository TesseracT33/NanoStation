#include "mmi.hpp"
#include "ee.hpp"
#include "host.hpp"

#include <bit>
#include <cassert>
#include <cstring>
#include <utility>

// reference: https://wiki.qemu.org/File:C790.pdf

namespace ee {
void madd(u32 rs, u32 rt, u32 rd) // Multiply/Add
{
    s64 res = s64(gpr[rs].s32()) * s64(gpr[rs].s32()) + lo.s32() + (hi.s64() << 32);
    gpr.set(rd, s32(res));
    lo = s32(res);
    hi = s32(res >> 32);
}

void madd1(u32 rs, u32 rt, u32 rd) // Multiply-Add Pipeline 1
{
}

void maddu(u32 rs, u32 rt, u32 rd) // Multiply/Add Unsigned
{
    u64 res = u64(gpr[rs].u32()) * u64(gpr[rs].u32()) + lo.u32() + (hi.u64() << 32);
    gpr.set(rd, s32(res));
    lo = s32(res);
    hi = s32(res >> 32);
}

void maddu1(u32 rs, u32 rt, u32 rd) // Multiply-Add Unsigned Pipeline 1
{
}

void pabsh(u32 rt, u32 rd) // Parallel Absolute Halfword
{
#if X64
    gpr[rd] = _mm_abs_epi16(gpr[rt]);
#endif
}

void pabsw(u32 rt, u32 rd) // Parallel Absolute Word
{
#if X64
    gpr[rd] = _mm_abs_epi32(gpr[rt]);
#endif
}

void paddb(u32 rs, u32 rt, u32 rd) // Parallel Add Byte
{
#if X64
    gpr[rd] = _mm_add_epi8(gpr[rs], gpr[rt]);
#endif
}

void paddh(u32 rs, u32 rt, u32 rd) // Parallel Add Halfword
{
#if X64
    gpr[rd] = _mm_add_epi16(gpr[rs], gpr[rt]);
#endif
}

void paddsb(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Byte
{
#if X64
    gpr[rd] = _mm_adds_epi8(gpr[rs], gpr[rt]);
#endif
}

void paddsh(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Halfword
{
#if X64
    gpr[rd] = _mm_adds_epi16(gpr[rs], gpr[rt]);
#endif
}

void paddsw(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Word
{
#if X64
    m128i ov, rt_neg, sat_sum, sum;
    sum = _mm_add_epi32(gpr[rs], gpr[rt]);
    ov = _mm_xor_si128(gpr[rs], sum);
    ov = _mm_and_si128(ov, _mm_xor_si128(gpr[rt], sum));
    ov = _mm_cmplt_epi32(ov, _mm_setzero_si128());
    rt_neg = _mm_cmplt_epi32(gpr[rt], _mm_setzero_si128());
    sat_sum = _mm_blendv_epi8(_mm_set1_epi32(0x7FFF'FFFF), _mm_set1_epi32(0x8000'0000), rt_neg);
    gpr[rd] = _mm_blendv_epi8(sum, sat_sum, ov);
#endif
}

void paddub(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Byte
{
#if X64
    gpr[rd] = _mm_adds_epu8(gpr[rs], gpr[rt]);
#endif
}

void padduh(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Halfword
{
#if X64
    gpr[rd] = _mm_adds_epu16(gpr[rs], gpr[rt]);
#endif
}

void padduw(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Word
{
#if X64
    gpr[rd] = _mm_add_epi32(gpr[rt], _mm_min_epu32(gpr[rs], _mm_xor_si128(gpr[rt], _mm_set1_epi8(0xFF))));
#endif
}

void paddw(u32 rs, u32 rt, u32 rd) // Parallel Add Word
{
#if X64
    gpr[rd] = _mm_add_epi32(gpr[rs], gpr[rt]);
#endif
}

void padsbh(u32 rs, u32 rt, u32 rd) // Parallel Add/Subtract Halfword
{
#if X64
    m128i add = _mm_add_epi16(gpr[rs], gpr[rt]);
    m128i sub = _mm_sub_epi16(gpr[rs], gpr[rt]);
    gpr[rd] = _mm_blend_epi16(add, sub, 0xF);
#endif
}

void pand(u32 rs, u32 rt, u32 rd) // Parallel AND
{
#if X64
    gpr[rd] = _mm_and_si128(gpr[rs], gpr[rt]);
#endif
}

void pceqb(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Byte
{
#if X64
    gpr[rd] = _mm_cmpeq_epi8(gpr[rs], gpr[rt]);
#endif
}

void pceqh(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Halfword
{
#if X64
    gpr[rd] = _mm_cmpeq_epi16(gpr[rs], gpr[rt]);
#endif
}

void pceqw(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Word
{
#if X64
    gpr[rd] = _mm_cmpeq_epi32(gpr[rs], gpr[rt]);
#endif
}

void pcgtb(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Byte
{
#if X64
    gpr[rd] = _mm_cmpgt_epi8(gpr[rs], gpr[rt]);
#endif
}

void pcgth(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Halfword
{
#if X64
    gpr[rd] = _mm_cmpgt_epi16(gpr[rs], gpr[rt]);
#endif
}

void pcgtw(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Word
{
#if X64
    gpr[rd] = _mm_cmpgt_epi32(gpr[rs], gpr[rt]);
#endif
}

void pcpyh(u32 rt, u32 rd) // Parallel Copy Halfword
{
    gpr[rd] = gpr[rt];
}

void pcpyld(u32 rs, u32 rt, u32 rd) // Parallel Copy Lower Doubleword
{
#if X64
    m128i lo = gpr[rt];
    m128i hi = _mm_slli_si128(gpr[rs], 8);
    gpr[rd] = _mm_blend_epi16(hi, lo, 0xF);
#endif
}

void pcpyud(u32 rs, u32 rt, u32 rd) // Parallel Copy Upper Doubleword
{
#if X64
    m128i lo = _mm_srli_si128(gpr[rs], 8);
    m128i hi = gpr[rt];
    gpr[rd] = _mm_blend_epi16(hi, lo, 0xF);
#endif
}

void pdivbw(u32 rs, u32 rt) // Parallel Divide Broadcast Word
{
#if X64
    m128i src_rs = gpr[rs];
    m128i src_rt = gpr[rt];
    s32 quot[4];
    s32 rem[4];
    for (int i = 0; i < 4; ++i) {
        s32 op1, op2;
        std::memcpy(&op1, reinterpret_cast<u8*>(&src_rs) + 4 * i, 4);
        std::memcpy(&op2, reinterpret_cast<u8*>(&src_rt) + 4 * i, 4);
        if (op2 == 0) {
            quot[i] = op1 >= 0 ? -1 : 1;
            rem[i] = op1;
        } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
            quot[i] = op1;
            rem[i] = 0;
        } else [[likely]] {
            quot[i] = op1 / op2;
            rem[i] = op1 % op2;
        }
    }
    lo = _mm_set_epi32(quot[3], quot[2], quot[1], quot[0]);
    hi = _mm_set_epi32(rem[3], rem[2], rem[1], rem[0]);
#endif
}

void pdivuw(u32 rs, u32 rt) // Parallel Divide Unsigned Word
{
#if X64
    m128i src_rs = gpr[rs];
    m128i src_rt = gpr[rt];
    s32 quot[2];
    s32 rem[2];
    for (int i = 0; i < 2; ++i) {
        u32 op1, op2;
        std::memcpy(&op1, reinterpret_cast<u8*>(&src_rs) + 8 * i, 4);
        std::memcpy(&op2, reinterpret_cast<u8*>(&src_rt) + 8 * i, 4);
        if (op2 == 0) {
            quot[i] = -1;
            rem[i] = op1;
        } else {
            quot[i] = op1 / op2;
            rem[i] = op1 % op2;
        }
    }
    lo = _mm_set_epi64x(quot[1], quot[0]);
    hi = _mm_set_epi64x(rem[1], rem[0]);
#endif
}

void pdivw(u32 rs, u32 rt) // Parallel Divide Word
{
#if X64
    m128i src_rs = gpr[rs];
    m128i src_rt = gpr[rt];
    s32 quot[2];
    s32 rem[2];
    for (int i = 0; i < 2; ++i) {
        s32 op1, op2;
        std::memcpy(&op1, reinterpret_cast<u8*>(&src_rs) + 8 * i, 4);
        std::memcpy(&op2, reinterpret_cast<u8*>(&src_rt) + 8 * i, 4);
        if (op2 == 0) {
            quot[i] = op1 >= 0 ? -1 : 1;
            rem[i] = op1;
        } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
            quot[i] = op1;
            rem[i] = 0;
        } else [[likely]] {
            quot[i] = op1 / op2;
            rem[i] = op1 % op2;
        }
    }
    lo = _mm_set_epi64x(quot[1], quot[0]);
    hi = _mm_set_epi64x(rem[1], rem[0]);
#endif
}

void pexch(u32 rt, u32 rd) // Parallel Exchange Center Halfword
{
#if X64
    gpr[rd] = _mm_shuffle_epi8(gpr[rt], _mm_set_epi8(15, 14, 11, 10, 13, 12, 9, 8, 7, 6, 3, 2, 5, 4, 1, 0));
#endif
}

void pexcw(u32 rt, u32 rd) // Parallel Exchange Center Word
{
#if X64
    gpr[rd] = _mm_shuffle_epi32(gpr[rt], 2 << 2 | 1 << 4 | 3 << 6);
#endif
}

void pexeh(u32 rt, u32 rd) // Parallel Exchange Even Halfword
{
#if X64
    gpr[rd] = _mm_shuffle_epi8(gpr[rt], _mm_set_epi8(15, 14, 9, 8, 11, 10, 13, 12, 7, 6, 1, 0, 3, 2, 5, 4));
#endif
}

void pexew(u32 rt, u32 rd) // Parallel Exchange Even Word
{
#if X64
    gpr[rd] = _mm_shuffle_epi32(gpr[rt], 2 | 1 << 2 | 3 << 6);
#endif
}

void pext5(u32 rt, u32 rd) // Parallel Extend Upper from 5 bits
{
#if X64
    m128i src = gpr[rt];
    u32 dst_w[4];
    for (int i = 0; i < 4; ++i) {
        u16 src_hw;
        std::memcpy(&src_hw, reinterpret_cast<u8*>(&src) + 4 * i, 2);
        dst_w[i] =
          ((src_hw & 0x1F) << 3) | ((src_hw & 0x3E0) << 6) | ((src_hw & 0x7C00) << 9) | ((src_hw & 0x8000) << 16);
    }
    gpr[rd] = _mm_set_epi32(dst_w[3], dst_w[2], dst_w[1], dst_w[0]);
#endif
}

void pextlb(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Byte
{
#if X64
    gpr[rd] = _mm_unpacklo_epi8(gpr[rt], gpr[rs]);
#endif
}

void pextlh(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Halfword
{
#if X64
    gpr[rd] = _mm_unpacklo_epi16(gpr[rt], gpr[rs]);
#endif
}

void pextlw(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Word
{
#if X64
    gpr[rd] = _mm_unpacklo_epi32(gpr[rt], gpr[rs]);
#endif
}

void pextub(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Byte
{
#if X64
    gpr[rd] = _mm_unpackhi_epi8(gpr[rt], gpr[rs]);
#endif
}

void pextuh(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Halfword
{
#if X64
    gpr[rd] = _mm_unpackhi_epi16(gpr[rt], gpr[rs]);
#endif
}

void pextuw(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Word
{
#if X64
    gpr[rd] = _mm_unpackhi_epi32(gpr[rt], gpr[rs]);
#endif
}

void phmadh(u32 rs, u32 rt, u32 rd) // Parallel Horizontal Multiply-Add Halfword
{
#if X64
    gpr[rd] = _mm_madd_epi16(gpr[rs], gpr[rt]);
    lo = gpr[rd]; // TODO: LO[63..32], LO[127..96] undefined. Better to sign-extend prod0 and prod2?
    hi = _mm_srli_si128(gpr[rd], 4); // TODO: HI[127..96], HI[63..32] undefined. Better to sign-extend prod1 and prod3?
#endif
}

void phmsbh(u32 rs, u32 rt, u32 rd) // Parallel Horizontal Multiply-Subtract Halfword
{
#if X64
    m128i src_rs = gpr[rs];
    m128i src_rt = gpr[rt];
    s32 prod[4];
    for (int i = 0; i < 4; ++i) {
        s16 op0, op1, op2, op3;
        std::memcpy(&op2, reinterpret_cast<u8*>(&src_rs) + 4 * i + 2, 2);
        std::memcpy(&op3, reinterpret_cast<u8*>(&src_rt) + 4 * i + 2, 2);
        std::memcpy(&op0, reinterpret_cast<u8*>(&src_rs) + 4 * i, 2);
        std::memcpy(&op1, reinterpret_cast<u8*>(&src_rt) + 4 * i, 2);
        prod[i] = op0 * op1 - op2 * op3;
    }
    gpr[rd] = _mm_set_epi32(prod[3], prod[2], prod[1], prod[0]);
    lo.set_lower_dword(prod[0]);
    lo.set_upper_dword(prod[2]);
    hi.set_lower_dword(prod[1]);
    hi.set_upper_dword(prod[3]);
#endif
}

void pinteh(u32 rs, u32 rt, u32 rd) // Parallel Interleave Even Halfword
{
#if X64
    m128i op1 = _mm_slli_si128(gpr[rs], 2);
    m128i op2 = gpr[rt];
    gpr[rd] = _mm_blend_epi16(op1, op2, 0x55);
#endif
}

void pinth(u32 rs, u32 rt, u32 rd) // Parallel Interleave Halfword
{
#if X64
    gpr[rd] = _mm_unpacklo_epi16(gpr[rt], _mm_srli_si128(gpr[rs], 8));
#endif
}

void plzcw(u32 rs, u32 rd) // Parallel Leading Zero or One Count Word
{
    u32 leading_ones = std::countl_one(gpr[rs].u32()) - 1; // TODO: should it be able to underflow?
    u32 leading_zeroes = std::countl_zero(gpr[rs].u64() >> 32) - 1;
    gpr.set(rd, u64(leading_zeroes) << 32 | leading_ones);
}

void pmaddh(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Halfword
{
}

void pmadduw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Unsigned Word
{
}

void pmaddw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Word
{
}

void pmaxh(u32 rs, u32 rt, u32 rd) // Parallel Maximum Halfword
{
#if X64
    gpr[rd] = _mm_max_epi16(gpr[rs], gpr[rt]);
#endif
}

void pmaxw(u32 rs, u32 rt, u32 rd) //  Parallel Maximum Word
{
#if X64
    gpr[rd] = _mm_max_epi32(gpr[rs], gpr[rt]);
#endif
}

void pmfhi(u32 rd) //  Parallel Move From HI Register
{
    gpr.set(rd, hi);
}

void pmfhl(u32 rd, u32 fmt) // Parallel Move From HI/LO Register
{
}

void pmflo(u32 rd) // Parallel Move From LO Register
{
    gpr.set(rd, lo);
}

void pminh(u32 rs, u32 rt, u32 rd) // Parallel Minimum Halfword
{
#if X64
    gpr[rd] = _mm_min_epi16(gpr[rs], gpr[rt]);
#endif
}

void pminw(u32 rs, u32 rt, u32 rd) // Parallel Minimum Word
{
#if X64
    gpr[rd] = _mm_min_epi32(gpr[rs], gpr[rt]);
#endif
}

void pmsubh(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Subtract Halfword
{
}

void pmsubw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Subtract Word
{
}

void pmthi(u32 rs) // Parallel Move To HI Register
{
    hi = gpr[rs];
}

void pmthl(u32 rs, u32 fmt) // Parallel Move To HI/LO Register
{
}

void pmtlo(u32 rs) // Parallel Move To LO Register
{
    lo = gpr[rs];
}

void pmulth(u32 rs, u32 rt, u32 rd) // Parallel Multiply Halfword
{
#if X64
    m128i prod_lo = _mm_mullo_epi16(gpr[rs], gpr[rt]);
    m128i prod_hi = _mm_mulhi_epi16(gpr[rs], gpr[rt]);
    gpr[rd] = _mm_blend_epi16(prod_lo, _mm_slli_si128(prod_hi, 2), 0xAA);
// TODO: lo, hi
#endif
}

void pmultuw(u32 rs, u32 rt, u32 rd) // Parallel Multiply Unsigned Word
{
#if X64
    m128i prod = _mm_mul_epu32(gpr[rs], gpr[rt]);
    gpr.set(rd, prod);
    lo = _mm_cvtepi32_epi64(prod);
    hi = _mm_cvtepi32_epi64(_mm_srli_epi64(prod, 32));
#endif
}

void pmultw(u32 rs, u32 rt, u32 rd) // Parallel Multiply Word
{
#if X64
    m128i prod = _mm_mul_epi32(gpr[rs], gpr[rt]);
    gpr.set(rd, prod);
    lo = _mm_cvtepi32_epi64(prod);
    hi = _mm_cvtepi32_epi64(_mm_srli_epi64(prod, 32));
#endif
}

void pnor(u32 rs, u32 rt, u32 rd) // Parallel NOR
{
#if X64
    gpr[rd] = _mm_xor_si128(_mm_or_si128(gpr[rs], gpr[rt]), _mm_set1_epi8(0xFF));
#endif
}

void por(u32 rs, u32 rt, u32 rd) // Parallel OR
{
#if X64
    gpr[rd] = _mm_or_si128(gpr[rs], gpr[rt]);
#endif
}

void ppac5(u32 rt, u32 rd) // Parallel Pack to 5 bits
{
    m128i src = gpr[rt];
    u16 dst_hw[4];
    for (int i = 0; i < 4; ++i) {
        u32 src_w;
        std::memcpy(&src_w, reinterpret_cast<u8*>(&src) + 4 * i, 4);
        dst_hw[i] = (src_w >> 3 & 0x1F) | (src_w >> 6 & 0x3E0) | (src_w >> 9 & 0x7C00) | (src_w >> 16 & 0x8000);
    }
    gpr[rd] = _mm_set_epi16(0, dst_hw[3], 0, dst_hw[2], 0, dst_hw[1], 0, dst_hw[0]);
}

void ppacb(u32 rs, u32 rt, u32 rd) // Parallel Pack to Byte
{
#if X64
    m128i lo = _mm_shuffle_epi8(gpr[rt], _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 14, 12, 10, 8, 6, 4, 2, 0));
    m128i hi = _mm_shuffle_epi8(gpr[rs], _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    gpr[rd] = _mm_blend_epi16(hi, lo, 0xF);
#endif
}

void ppach(u32 rs, u32 rt, u32 rd) // Parallel Pack to Halfword
{
#if X64
    m128i lo = _mm_shuffle_epi8(gpr[rt], _mm_set_epi8(0, 0, 0, 0, 0, 0, 0, 0, 13, 12, 9, 8, 5, 4, 1, 0));
    m128i hi = _mm_shuffle_epi8(gpr[rs], _mm_set_epi8(13, 12, 9, 8, 5, 4, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    gpr[rd] = _mm_blend_epi16(hi, lo, 0xF);
#endif
}

void ppacw(u32 rs, u32 rt, u32 rd) // Parallel Pack to Word
{
#if X64
    m128i lo = _mm_shuffle_epi32(gpr[rt], 2 << 2);
    m128i hi = _mm_shuffle_epi32(gpr[rs], 2 << 6);
    gpr[rd] = _mm_blend_epi16(hi, lo, 0xF);
#endif
}

void prevh(u32 rt, u32 rd) // Parallel Reverse Halfword
{
#if X64
    gpr[rd] = _mm_shuffle_epi8(gpr[rt], _mm_set_epi8(9, 8, 11, 10, 13, 12, 15, 14, 1, 0, 3, 2, 5, 4, 7, 6));
#endif
}

void prot3w(u32 rt, u32 rd) // Parallel Rotate 3 Words
{
#if X64
    gpr[rd] = _mm_shuffle_epi32(gpr[rt], 1 | 2 << 2 | 3 << 6);
#endif
}

void psllh(u32 rt, u32 rd, u32 sa) // Parallel Shift Left Logical Halfword
{
#if X64
    gpr[rd] = _mm_slli_epi16(gpr[rt], sa);
#endif
}

void psllvw(u32 rs, u32 rt, u32 rd) // Parallel Shift Left Logical Variable Word
{
#if X64
    gpr[rd] = _mm_sll_epi32(gpr[rt], gpr[rs]);
#endif
}

void psllw(u32 rt, u32 rd, u32 sa) // Parallel Shift Left Logical Word
{
#if X64
    gpr[rd] = _mm_slli_epi32(gpr[rt], sa);
#endif
}

void psrah(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Arithmetic Halfword
{
#if X64
    gpr[rd] = _mm_srai_epi16(gpr[rt], sa);
#endif
}

void psravw(u32 rs, u32 rt, u32 rd) // Parallel Shift Right Arithmetic Variable Word
{
#if X64
    gpr[rd] = _mm_sra_epi32(gpr[rt], gpr[rs]);
#endif
}

void psraw(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Arithmetic Word
{
#if X64
    gpr[rd] = _mm_srai_epi32(gpr[rt], sa);
#endif
}

void psrlh(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Logical Halfword
{
#if X64
    gpr[rd] = _mm_srli_epi16(gpr[rt], sa);
#endif
}

void psrlvw(u32 rs, u32 rt, u32 rd) // Parallel Shift Right Logical Variable Word
{
#if X64
    gpr[rd] = _mm_srl_epi32(gpr[rt], gpr[rs]);
#endif
}

void psrlw(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Logical Word
{
#if X64
    gpr[rd] = _mm_srli_epi32(gpr[rt], sa);
#endif
}

void psubb(u32 rs, u32 rt, u32 rd) // Parallel Subtract Byte
{
#if X64
    gpr[rd] = _mm_sub_epi8(gpr[rs], gpr[rt]);
#endif
}

void psubh(u32 rs, u32 rt, u32 rd) // Parallel Subtract Halfword
{
#if X64
    gpr[rd] = _mm_sub_epi16(gpr[rs], gpr[rt]);
#endif
}

void psubsb(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Byte
{
#if X64
    gpr[rd] = _mm_subs_epi8(gpr[rs], gpr[rt]);
#endif
}

void psubsh(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Halfword
{
#if X64
    gpr[rd] = _mm_subs_epi16(gpr[rs], gpr[rt]);
#endif
}

void psubsw(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Word
{
#if X64
    m128i ov, rt_neg, sat_sub, sub;
    sub = _mm_sub_epi32(gpr[rs], gpr[rt]);
    ov = _mm_xor_si128(gpr[rs], gpr[rt]);
    ov = _mm_andnot_si128(_mm_xor_si128(gpr[rt], sub), ov);
    ov = _mm_cmplt_epi32(ov, _mm_setzero_si128());
    rt_neg = _mm_cmplt_epi32(gpr[rt], _mm_setzero_si128());
    sat_sub = _mm_blendv_epi8(_mm_set1_epi32(0x8000'0000), _mm_set1_epi32(0x7FFF'FFFF), rt_neg);
    gpr[rd] = _mm_blendv_epi8(sub, sat_sub, ov);
#endif
}

void psubub(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Byte
{
#if X64
    gpr[rd] = _mm_subs_epu8(gpr[rs], gpr[rt]);
#endif
}

void psubuh(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Halfword
{
#if X64
    gpr[rd] = _mm_subs_epu16(gpr[rs], gpr[rt]);
#endif
}

void psubuw(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Word
{
#if X64
    gpr[rd] = _mm_sub_epi32(_mm_max_epu32(gpr[rs], gpr[rt]), gpr[rt]);
#endif
}

void psubw(u32 rs, u32 rt, u32 rd) // Parallel Subtract Word
{
#if X64
    gpr[rd] = _mm_sub_epi32(gpr[rs], gpr[rt]);
#endif
}

void pxor(u32 rs, u32 rt, u32 rd) // Parallel XOR
{
#if X64
    gpr[rd] = _mm_xor_si128(gpr[rs], gpr[rt]);
#endif
}

void qfsrv(u32 rs, u32 rt, u32 rd) // Quadword Funnel Shift Right Variable
{
    assert(!(sa & 7));
    if (sa > 255) {
        std::memset(&gpr[rd], 0, 16);
    } else if (sa > 0) [[likely]] {
#if INT128_AVAILABLE
        using u128 = __uint128_t;
        u128 rs_src = std::bit_cast<u128>(gpr[rs]);
        u128 rt_src = std::bit_cast<u128>(gpr[rt]);
        u128 res;
        if (sa < 128) {
            res = rt_src >> sa | rs_src << (128 - sa);
        } else {
            res = rs_src >> (sa - 128);
        }
        gpr.set(rd, std::bit_cast<m128i>(res));
#else
        m128i qwords[2] = { gpr[rt], gpr[rs] };
        u64 dwords[4];
        std::memcpy(dwords, qwords, 32);
        u64 res[2];
        u64 q = sa / 64;
        u64 rshift = sa & 63;
        if (rshift == 0) {
            res[0] = dwords[q];
            res[1] = q < 3 ? dwords[q + 1] : 0;
        } else {
            res[0] = dwords[q] >> rshift;
            if (q < 3) {
                u64 lshift = 64 - rshift;
                res[0] |= dwords[q + 1] << lshift;
                res[1] = dwords[q + 1] >> rshift;
                if (q < 2) {
                    res[1] |= dwords[q + 2] << lshift;
                }
            } else {
                res[1] = 0;
            }
        }
        m128i out;
        std::memcpy(&out, res, 16);
        gpr[rd] = out;
#endif
    } else {
        gpr[rd] = gpr[rt];
    }
}

} // namespace ee
