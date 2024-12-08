#include "mmi.hpp"
#include "asmjit/x86/x86operand.h"
#include "ee.hpp"

#include "asmjit/x86.h"
#include "jit.hpp"
#include "jit_utils.hpp"
#include "platform.hpp"
#include <algorithm>
#include <bit>
#include <cstring>

#include <immintrin.h>
#include <limits>
#include <tuple>

// reference: https://wiki.qemu.org/File:C790.pdf
// optimizations: https://godbolt.org/z/KEz81v9zz

using namespace asmjit::x86;

namespace ee {

static std::tuple<Xmm, Xmm, Xmm> get_vpr_rd_rs_rt(u32 rd, u32 rs, u32 rt)
{
    return { reg_alloc.GetDirtyVpr(rd), reg_alloc.GetVpr(rs), reg_alloc.GetVpr(rt) };
}

static asmjit::x86::Xmm get_vpr(u32 index)
{
    (void)index;
    return xmm0;
}

static asmjit::x86::Xmm get_dirty_vpr(u32 index)
{
    (void)index;
    return xmm0;
}

static asmjit::x86::Gpq get_gpr(u32 index)
{
    (void)index;
    return rax;
}

static asmjit::x86::Gpq get_dirty_gpr(u32 index)
{
    (void)index;
    return rax;
}

static Xmm get_lo()
{
    return xmm0;
}

static Xmm get_hi()
{
    return xmm0;
}

static Xmm get_dirty_lo()
{
    return xmm0;
}

static Xmm get_dirty_hi()
{
    return xmm0;
}

static Mem jit_ptr(auto)
{
    return {};
}

void pabsh(u32 rt, u32 rd) // Parallel Absolute Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpabsw(hd, ht);
}

void pabsw(u32 rt, u32 rd) // Parallel Absolute Word
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpabsd(hd, ht);
}

void paddb(u32 rs, u32 rt, u32 rd) // Parallel Add Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddb(hd, hs, ht);
}

void paddh(u32 rs, u32 rt, u32 rd) // Parallel Add Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddw(hd, hs, ht);
}

void paddsb(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddsb(hd, hs, ht);
}

void paddsh(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddsw(hd, hs, ht);
}

void paddsw(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Word
{
    static constexpr u32 mask = 0x8000'0000;
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpbroadcastd(xmm0, jit_ptr(mask));
    c.vpaddd(xmm1, hs, ht);
    c.vpcmpgtd(xmm2, hs, xmm1);
    c.vpxor(xmm2, ht, xmm2);
    c.vpsrad(xmm3, xmm1, 31);
    c.vpxor(xmm3, xmm3, xmm0);
    c.vblendvps(hd, xmm1, xmm3, xmm2);
}

void paddub(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddusb(hd, hs, ht);
}

void padduh(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddusw(hd, hs, ht);
}

void padduw(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpeqd(xmm0, xmm0, xmm0);
    c.vpxor(xmm0, xmm0, ht);
    c.vpminud(xmm0, xmm0, hs);
    c.vpaddd(hd, xmm0, ht);
}

void paddw(u32 rs, u32 rt, u32 rd) // Parallel Add Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddd(hd, hs, ht);
}

void padsbh(u32 rs, u32 rt, u32 rd) // Parallel Add/Subtract Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpaddw(xmm0, hs, ht);
    c.vpsubw(xmm1, hs, ht);
    c.vpblendw(hd, xmm1, xmm0, 15);
}

void pand(u32 rs, u32 rt, u32 rd) // Parallel AND
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpand(hd, hs, ht);
}

void pceqb(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpeqb(hd, hs, ht);
}

void pceqh(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpeqw(hd, hs, ht);
}

void pceqw(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpeqd(hd, hs, ht);
}

void pcgtb(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpgtb(hd, hs, ht);
}

void pcgth(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpgtw(hd, hs, ht);
}

void pcgtw(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpcmpgtd(hd, hs, ht);
}

void pcpyh(u32 rt, u32 rd) // Parallel Copy Halfword
{
    static constexpr u8 mask[] = { 0, 1, 0, 1, 0, 1, 0, 1, 8, 9, 8, 9, 8, 9, 8, 9 };
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshufb(hd, ht, jit_ptr(mask));
}

void pcpyld(u32 rs, u32 rt, u32 rd) // Parallel Copy Lower Doubleword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpcklqdq(hd, ht, hs);
}

void pcpyud(u32 rs, u32 rt, u32 rd) // Parallel Copy Upper Doubleword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpckhqdq(hd, hs, ht);
}

void pdivbw(u32 rs, u32 rt) // Parallel Divide Broadcast Word
{
    auto do_div = [](u32 rs, u32 rt) {
        s32 quot[4];
        s32 rem[4];
        s32 op1;
        s16 op2 = s16(gpr[rt]);
        for (int i = 0; i < 4; ++i) {
            std::memcpy(&op1, reinterpret_cast<u8*>(&gpr[rs]) + 4 * i, 4);
            if (op2 == 0) {
                quot[i] = op1 < 0 ? 1 : -1;
                rem[i] = op1;
            } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
                quot[i] = op1;
                rem[i] = 0;
            } else [[likely]] {
                quot[i] = op1 / op2;
                rem[i] = u16(op1 % op2);
            }
        }
        lo = std::bit_cast<u128>(_mm_set_epi32(quot[3], quot[2], quot[1], quot[0]));
        hi = std::bit_cast<u128>(_mm_set_epi32(rem[3], rem[2], rem[1], rem[0]));
    };
    reg_alloc.FlushAll();
    if constexpr (platform.abi.systemv) {
        c.push(rax);
        c.mov(edi, rs);
        c.mov(esi, rt);
        c.call(+do_div);
        c.pop(rcx);
    }
    if constexpr (platform.abi.win64) {
        c.sub(rsp, 40);
        c.mov(cl, rs);
        c.mov(dl, rt);
        c.call(+do_div);
        c.add(rsp, 40);
    }
}

void pdivuw(u32 rs, u32 rt) // Parallel Divide Unsigned Word
{
    auto do_div = [](u32 rs, u32 rt) {
        s32 quot[2];
        s32 rem[2];
        for (int i = 0; i < 2; ++i) {
            u32 op1, op2;
            std::memcpy(&op1, reinterpret_cast<u8*>(&gpr[rs]) + 8 * i, 4);
            std::memcpy(&op2, reinterpret_cast<u8*>(&gpr[rt]) + 8 * i, 4);
            if (op2 == 0) {
                quot[i] = -1;
                rem[i] = op1;
            } else {
                quot[i] = op1 / op2;
                rem[i] = op1 % op2;
            }
        }
        lo = std::bit_cast<u128>(_mm_set_epi64x(quot[1], quot[0]));
        hi = std::bit_cast<u128>(_mm_set_epi64x(rem[1], rem[0]));
    };
    reg_alloc.FlushAll();
    if constexpr (platform.abi.systemv) {
        c.push(rax);
        c.mov(edi, rs);
        c.mov(esi, rt);
        c.call(+do_div);
        c.pop(rcx);
    }
    if constexpr (platform.abi.win64) {
        c.sub(rsp, 40);
        c.mov(cl, rs);
        c.mov(dl, rt);
        c.call(+do_div);
        c.add(rsp, 40);
    }
}

void pdivw(u32 rs, u32 rt) // Parallel Divide Word
{
    auto do_div = [](u32 rs, u32 rt) {
        s32 quot[2];
        s32 rem[2];
        for (int i = 0; i < 2; ++i) {
            s32 op1, op2;
            std::memcpy(&op1, reinterpret_cast<u8*>(&gpr[rs]) + 8 * i, 4);
            std::memcpy(&op2, reinterpret_cast<u8*>(&gpr[rt]) + 8 * i, 4);
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
        lo = std::bit_cast<u128>(_mm_set_epi64x(quot[1], quot[0]));
        hi = std::bit_cast<u128>(_mm_set_epi64x(rem[1], rem[0]));
    };
    reg_alloc.FlushAll();
    if constexpr (platform.abi.systemv) {
        c.push(rax);
        c.mov(edi, rs);
        c.mov(esi, rt);
        c.call(+do_div);
        c.pop(rcx);
    }
    if constexpr (platform.abi.win64) {
        c.sub(rsp, 40);
        c.mov(cl, rs);
        c.mov(dl, rt);
        c.call(+do_div);
        c.add(rsp, 40);
    }
}

void pexch(u32 rt, u32 rd) // Parallel Exchange Center Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshuflw(hd, ht, 2 << 2 | 1 << 4 | 3 << 6);
    c.vpshufhw(hd, ht, 2 << 2 | 1 << 4 | 3 << 6);
}

void pexcw(u32 rt, u32 rd) // Parallel Exchange Center Word
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshufd(hd, ht, 2 << 2 | 1 << 4 | 3 << 6);
}

void pexeh(u32 rt, u32 rd) // Parallel Exchange Even Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshuflw(hd, ht, 2 | 1 << 2 | 3 << 6);
    c.vpshufhw(hd, ht, 2 | 1 << 2 | 3 << 6);
}

void pexew(u32 rt, u32 rd) // Parallel Exchange Even Word
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshufd(hd, ht, 2 | 1 << 2 | 3 << 6);
}

void pext5(u32 rt, u32 rd) // Parallel Extend Upper from 5 bits
{
    static constexpr u8 shuf_mask[16] = { 0, 1, 4, 5, 8, 9, 12, 13, 0, 0, 0, 0, 0, 0, 0, 0 };
    static constexpr u64 pdep_mask = 0x80f8'f8f8'80f8'f8f8;
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshufb(xmm0, ht, jit_ptr(shuf_mask));
    c.vmovq(rax, xmm0);
    c.pdep(rcx, rax, jit_ptr(pdep_mask));
    c.vmovq(hd, rcx);
    c.shr(rax, 32);
    c.pdep(rcx, rax, jit_ptr(pdep_mask));
    c.vpinsrq(hd, hd, rcx, 1);
}

void pextlb(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpcklbw(hd, ht, hs);
}

void pextlh(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpcklwd(hd, ht, hs);
}

void pextlw(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpckldq(hd, ht, hs);
}

void pextub(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpckhbw(hd, ht, hs);
}

void pextuh(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpckhwd(hd, ht, hs);
}

void pextuw(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpunpckhdq(hd, ht, hs);
}

void phmadh(u32 rs, u32 rt, u32 rd) // Parallel Horizontal Multiply-Add Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    Xmm lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmaddwd(hd, hs, ht);
    c.vmovaps(lo, hd);
    c.vpsrlq(hi, hd, 4);
}

void phmsbh(u32 rs, u32 rt, u32 rd) // Parallel Horizontal Multiply-Subtract Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    Xmm lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpxor(xmm0, xmm0, xmm0);
    c.vpblendw(xmm0, hs, xmm0, 0xaa);
    c.vpsrld(xmm1, hs, 16);
    c.vpmaddwd(xmm0, ht, xmm0);
    c.vpsrld(xmm2, ht, 16);
    c.vpmaddwd(xmm1, xmm2, xmm1);
    c.vpsubd(hd, xmm0, xmm1);
    c.vmovaps(lo, hd);
    c.vpsrlq(hi, hd, 4);
}

void pinteh(u32 rs, u32 rt, u32 rd) // Parallel Interleave Even Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpslldq(xmm0, hs, 2);
    c.vpblendw(hd, xmm0, ht, 0x55);
}

void pinth(u32 rs, u32 rt, u32 rd) // Parallel Interleave Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsrldq(xmm0, hs, 8);
    c.vpunpcklwd(hd, ht, xmm0);
}

void plzcw(u32 rs, u32 rd) // Parallel Leading Zero or One Count Word
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs);
    c.mov(rax, hs);
    c.mov(ecx, hs.r32());
    c.mov(rdx, hs);
    c.shr(rax, 32);
    c.shr(rdx, 32);
    c.not_(ecx);
    c.not_(edx);
    c.lzcnt(hd.r32(), hs.r32());
    c.lzcnt(eax, eax);
    c.lzcnt(ecx, ecx);
    c.cmovnz(hd.r32(), ecx);
    c.lzcnt(edx, edx);
    c.cmovnz(eax, edx);
    c.dec(hd.r32());
    c.dec(eax);
    c.shl(rax, 32);
    c.or_(hd, rax);
}

void pmaddh(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Halfword
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmullw(xmm0, hs, ht);
    c.vpmulhw(xmm1, hs, ht);
    c.vpunpcklwd(xmm2, xmm0, xmm1);
    c.vpunpckhwd(xmm3, xmm0, xmm1);
    c.vpunpcklqdq(xmm0, lo, hi);
    c.vpunpckhqdq(xmm1, lo, hi);
    c.vpaddd(xmm0, xmm0, xmm2);
    c.vpaddd(xmm1, xmm1, xmm3);
    c.vshufps(hd, xmm0, xmm1, 2 << 2 | 2 << 6);
    c.vmovlhps(lo, xmm0, xmm1);
    c.vunpckhpd(hi, xmm0, xmm1);
}

void pmadduw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Unsigned Word
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmuludq(xmm0, hs, ht);
    c.vpslldq(xmm1, hi, 4);
    c.vpblendd(xmm1, xmm1, lo, 5);
    c.vpaddq(hd, xmm0, xmm1);
    c.vpshufd(xmm0, hd, 2 << 2);
    c.vpshufd(xmm1, hd, 1 | 3 << 2);
    c.vpmovsxdq(lo, xmm0);
    c.vpmovsxdq(hi, xmm1);
}

void pmaddw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Word
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmuldq(xmm0, hs, ht);
    c.vpslldq(xmm1, hi, 4);
    c.vpblendd(xmm1, xmm1, lo, 5);
    c.vpaddq(hd, xmm0, xmm1);
    c.vpshufd(xmm0, hd, 2 << 2);
    c.vpshufd(xmm1, hd, 1 | 3 << 2);
    c.vpmovsxdq(lo, xmm0);
    c.vpmovsxdq(hi, xmm1);
}

void pmaxh(u32 rs, u32 rt, u32 rd) // Parallel Maximum Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpmaxsw(hd, hs, ht);
}

void pmaxw(u32 rs, u32 rt, u32 rd) //  Parallel Maximum Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpmaxsd(hd, hs, ht);
}

void pmfhi(u32 rd) //  Parallel Move From HI Register
{
    Xmm hd = get_dirty_vpr(rd), hi = get_hi();
    c.vmovaps(hd, hi);
}

void pmfhl(u32 rd, u32 fmt) // Parallel Move From HI/LO Register
{
    Xmm hd = get_dirty_vpr(rd), lo = get_lo(), hi = get_hi();
    if (fmt == 0) {
        c.vpslldq(xmm0, hi, 4);
        c.vpblendd(hd, xmm0, lo, 5);
    }
    if (fmt == 1) {
        c.vpsrldq(xmm0, lo, 4);
        c.vpblendd(hd, hi, xmm0, 5);
    }
    if (fmt == 2) {
        static constexpr s64 mask_min = std::numeric_limits<s32>::min();
        static constexpr s64 mask_max = std::numeric_limits<s32>::max();
        c.vpbroadcastq(xmm0, jit_ptr(mask_min));
        c.vpbroadcastq(xmm1, jit_ptr(mask_max));
        c.vpslldq(xmm2, hi, 4);
        c.vpblendd(xmm2, xmm2, lo, 5);
        c.vpcmpgtq(xmm3, xmm2, xmm0);
        c.vblendvpd(xmm2, xmm0, xmm2, xmm3);
        c.vpcmpgtq(xmm3, xmm1, xmm2);
        c.vblendvpd(hd, xmm1, xmm2, xmm3);
    }
    if (fmt == 3) {
        c.vpxor(xmm0, xmm0, xmm0);
        c.vpblendw(xmm1, xmm0, lo, 0x55);
        c.vpblendw(xmm2, xmm0, hi, 0x55);
        c.vpackusdw(xmm0, xmm1, xmm2);
        c.vpshufd(hd, xmm0, 2 << 2 | 1 << 4 | 3 << 6);
    }
    if (fmt == 4) {
        c.vpackssdw(xmm0, lo, hi);
        c.vpshufd(hd, xmm0, 2 << 2 | 1 << 4 | 3 << 6);
    }
}

void pmflo(u32 rd) // Parallel Move From LO Register
{
    Xmm hd = get_dirty_vpr(rd), lo = get_lo();
    c.vmovaps(hd, lo);
}

void pminh(u32 rs, u32 rt, u32 rd) // Parallel Minimum Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpminsw(hd, hs, ht);
}

void pminw(u32 rs, u32 rt, u32 rd) // Parallel Minimum Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpminsd(hd, hs, ht);
}

void pmsubh(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Subtract Halfword
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmullw(xmm0, hs, ht);
    c.vpmulhw(xmm1, hs, ht);
    c.vpunpcklwd(xmm2, xmm0, xmm1);
    c.vpunpckhwd(xmm3, xmm0, xmm1);
    c.vpunpcklqdq(xmm0, lo, hi);
    c.vpunpckhqdq(xmm1, lo, hi);
    c.vpsubd(xmm0, xmm0, xmm2);
    c.vpsubd(xmm1, xmm1, xmm3);
    c.vshufps(hd, xmm0, xmm1, 2 << 2 | 2 << 6);
    c.vmovlhps(lo, xmm0, xmm1);
    c.vunpckhpd(hi, xmm0, xmm1);
}

void pmsubw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Subtract Word
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmuldq(xmm0, hs, ht);
    c.vpslldq(xmm1, hi, 4);
    c.vpblendd(xmm1, xmm1, lo, 5);
    c.vpsubq(hd, xmm1, xmm0);
    c.vpshufd(xmm0, hd, 2 << 2);
    c.vpshufd(xmm1, hd, 1 | 3 << 2);
    c.vpmovsxdq(lo, xmm0);
    c.vpmovsxdq(hi, xmm1);
}

void pmthi(u32 rs) // Parallel Move To HI Register
{
    Xmm hi = get_dirty_hi(), hs = get_vpr(rs);
    c.vmovaps(hi, hs);
}

void pmthl(u32 rs, u32 fmt) // Parallel Move To HI/LO Register
{
    if (fmt) return;
    Xmm lo = get_dirty_lo(), hi = get_dirty_hi(), hs = get_vpr(rs);
    c.vpsrldq(xmm0, hs, 4);
    c.vpblendd(lo, lo, hs, 5);
    c.vpblendd(hi, hi, xmm0, 5);
}

void pmtlo(u32 rs) // Parallel Move To LO Register
{
    Xmm lo = get_dirty_lo(), hs = get_vpr(rs);
    c.vmovaps(lo, hs);
}

void pmulth(u32 rs, u32 rt, u32 rd) // Parallel Multiply Halfword
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmullw(xmm0, hs, ht);
    c.vpmulhw(xmm1, hs, ht);
    c.vpunpcklwd(xmm2, xmm0, xmm1);
    c.vpunpckhwd(xmm3, xmm0, xmm1);
    c.vshufps(hd, xmm2, xmm3, 2 << 2 | 2 << 6);
    c.vshufps(lo, xmm2, xmm3, 1 << 2 | 1 << 6);
    c.vshufps(hi, xmm2, xmm3, 2 | 3 << 2 | 2 << 4 | 3 << 6);
}

void pmultuw(u32 rs, u32 rt, u32 rd) // Parallel Multiply Unsigned Word
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmuludq(hd, hs, ht);
    c.vpshufd(xmm0, hd, 2 << 2);
    c.vpshufd(xmm1, hd, 1 | 3 << 2);
    c.vpmovsxdq(lo, xmm0);
    c.vpmovsxdq(hi, xmm1);
}

void pmultw(u32 rs, u32 rt, u32 rd) // Parallel Multiply Word
{
    Xmm hd = get_dirty_vpr(rd), hs = get_vpr(rs), ht = get_vpr(rt), lo = get_dirty_lo(), hi = get_dirty_hi();
    c.vpmuldq(hd, hs, ht);
    c.vpshufd(xmm0, hd, 2 << 2);
    c.vpshufd(xmm1, hd, 1 | 3 << 2);
    c.vpmovsxdq(lo, xmm0);
    c.vpmovsxdq(hi, xmm1);
}

void pnor(u32 rs, u32 rt, u32 rd) // Parallel NOR
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpor(hd, hs, ht);
    c.vpcmpeqd(xmm0, xmm0, xmm0);
    c.vpxor(hd, hd, xmm0);
}

void por(u32 rs, u32 rt, u32 rd) // Parallel OR
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpor(hd, hs, ht);
}

void ppac5(u32 rt, u32 rd) // Parallel Pack to 5 bits
{
    static constexpr u8 shuf_mask[16] = { 0, 1, 0x80, 0x80, 2, 3, 0x80, 0x80, 4, 5, 0x80, 0x80, 6, 7, 0x80, 0x80 };
    static constexpr u64 pext_mask = 0x80f8'f8f8'80f8'f8f8;
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vmovq(rax, ht);
    c.pext(rcx, rax, jit_ptr(pext_mask));
    c.vmovq(xmm0, rcx);
    c.vpextrq(rax, ht, 1);
    c.pext(rcx, rax, jit_ptr(pext_mask));
    c.vpinsrq(xmm0, xmm0, rcx, 1);
    c.vpshufb(hd, xmm0, jit_ptr(shuf_mask));
}

void ppacb(u32 rs, u32 rt, u32 rd) // Parallel Pack to Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    static constexpr u8 mask1[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 4, 6, 8, 10, 12, 14 };
    static constexpr u8 mask2[16] = { 0, 2, 4, 6, 8, 10, 12, 14, 0, 0, 0, 0, 0, 0, 0, 0 };
    c.vpshufb(xmm0, hs, jit_ptr(mask1));
    c.vpshufb(xmm1, ht, jit_ptr(mask2));
    c.vpblendw(hd, xmm0, xmm1, 15);
}

void ppach(u32 rs, u32 rt, u32 rd) // Parallel Pack to Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpxor(xmm0, xmm0, xmm0);
    c.vpblendw(xmm1, xmm0, hs, 0x55);
    c.vpblendw(xmm2, xmm0, ht, 0x55);
    c.vpackusdw(hd, xmm2, xmm1);
}

void ppacw(u32 rs, u32 rt, u32 rd) // Parallel Pack to Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vshufps(hd, ht, hs, 2 << 2 | 2 << 6);
}

void prevh(u32 rt, u32 rd) // Parallel Reverse Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.pshuflw(hd, ht, 3 | 2 << 2 | 1 << 4);
    c.pshufhw(hd, ht, 3 | 2 << 2 | 1 << 4);
}

void prot3w(u32 rt, u32 rd) // Parallel Rotate 3 Words
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpshufd(hd, ht, 1 | 2 << 2 | 3 << 6);
}

void psllh(u32 rt, u32 rd, u32 sa) // Parallel Shift Left Logical Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpsllw(hd, ht, sa & 15);
}

void psllvw(u32 rs, u32 rt, u32 rd) // Parallel Shift Left Logical Variable Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpslld(xmm0, hs, 27);
    c.vpsrld(xmm0, xmm0, 27);
    c.vpsllvd(xmm0, ht, xmm0);
    c.vpshufd(xmm0, xmm0, 2 << 2);
    c.vpmovsxdq(hd, xmm0);
}

void psllw(u32 rt, u32 rd, u32 sa) // Parallel Shift Left Logical Word
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpslld(hd, ht, sa & 31);
}

void psrah(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Arithmetic Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpsraw(hd, ht, sa & 15);
}

void psravw(u32 rs, u32 rt, u32 rd) // Parallel Shift Right Arithmetic Variable Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpslld(xmm0, hs, 27);
    c.vpsrld(xmm0, xmm0, 27);
    c.vpsravd(xmm0, ht, xmm0);
    c.vpshufd(xmm0, xmm0, 2 << 2);
    c.vpmovsxdq(hd, xmm0);
}

void psraw(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Arithmetic Word
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpsrad(hd, ht, sa & 31);
}

void psrlh(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Logical Halfword
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpsrlw(hd, ht, sa & 15);
}

void psrlvw(u32 rs, u32 rt, u32 rd) // Parallel Shift Right Logical Variable Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpslld(xmm0, hs, 27);
    c.vpsrld(xmm0, xmm0, 27);
    c.vpsrlvd(xmm0, ht, xmm0);
    c.vpshufd(xmm0, xmm0, 2 << 2);
    c.vpmovsxdq(hd, xmm0);
}

void psrlw(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Logical Word
{
    Xmm hd = get_dirty_vpr(rd), ht = get_vpr(rt);
    c.vpsrld(hd, ht, sa & 31);
}

void psubb(u32 rs, u32 rt, u32 rd) // Parallel Subtract Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubb(hd, hs, ht);
}

void psubh(u32 rs, u32 rt, u32 rd) // Parallel Subtract Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubw(hd, hs, ht);
}

void psubsb(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubsb(hd, hs, ht);
}

void psubsh(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubsw(hd, hs, ht);
}

void psubsw(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Word
{
    static constexpr u32 mask = 0x8000'0000;
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpbroadcastd(xmm0, jit_ptr(mask));
    c.vpxor(xmm1, xmm1, xmm1);
    c.vpcmpgtd(xmm1, ht, xmm1);
    c.vpsubd(xmm2, hs, ht);
    c.vpcmpgtd(xmm3, hs, xmm2);
    c.vpxor(xmm3, xmm1, xmm3);
    c.vpsrad(xmm1, xmm2, 31);
    c.vpxor(xmm1, xmm1, xmm0);
    c.vblendvps(hd, xmm2, xmm1, xmm3);
}

void psubub(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Byte
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubusb(hd, hs, ht);
}

void psubuh(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Halfword
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubusw(hd, hs, ht);
}

void psubuw(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpmaxud(xmm0, hs, ht);
    c.vpsubd(hd, xmm0, ht);
}

void psubw(u32 rs, u32 rt, u32 rd) // Parallel Subtract Word
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpsubd(hd, hs, ht);
}

void pxor(u32 rs, u32 rt, u32 rd) // Parallel XOR
{
    auto [hd, hs, ht] = get_vpr_rd_rs_rt(rd, rs, rt);
    c.vpxor(hd, hs, ht);
}

void qfsrv(u32 rs, u32 rt, u32 rd) // Quadword Funnel Shift Right Variable
{
    if (!rd) return;
    auto do_qfsrv = [](u32 rs, u32 rt, u32 rd) {
        if (sa > 255) {
            gpr[rd] = 0;
        } else if (sa > 0) [[likely]] {
            u128 hs = std::bit_cast<u128>(gpr[rs]);
            u128 ht = std::bit_cast<u128>(gpr[rt]);
            u128 hd;
            if (sa < 128) {
                hd = ht >> sa | hs << (128 - sa);
            } else {
                hd = hs >> (sa - 128);
            }
            memcpy(&gpr[rd], &hd, 16);
        } else {
            gpr[rd] = gpr[rt];
        }
    };
    reg_alloc.FlushAll();
    if constexpr (platform.abi.systemv) {
        c.push(rax);
        c.mov(edi, rs);
        c.mov(esi, rt);
        c.mov(edx, rd);
        c.call(+do_qfsrv);
        c.pop(rcx);
    }
    if constexpr (platform.abi.win64) {
        c.sub(rsp, 40);
        c.mov(cl, rs);
        c.mov(dl, rt);
        c.mov(r8b, rd);
        c.call(+do_qfsrv);
        c.add(rsp, 40);
    }
}

} // namespace ee
