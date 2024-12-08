#include "cpu.hpp"
#include "asmjit/x86.h"
#include "ee.hpp"
#include "exceptions.hpp"
#include "jit.hpp"
#include "mmu.hpp"
#include <bit>

using namespace asmjit;
using namespace asmjit::x86;

namespace ee {

asmjit::Label l_early_block_epilogue;

asmjit::x86::Gpq get_gpr(u32 index)
{
    (void)index;
    return rax;
}

asmjit::x86::Gpq get_dirty_gpr(u32 index)
{
    (void)index;
    return rax;
}

Xmm get_dirty_lo()
{
    return xmm0;
}

Xmm get_dirty_hi()
{
    return xmm0;
}

void block_epilog_with_pc_flush_and_jmp(auto...)
{
}

asmjit::x86::Mem jit_ptr(auto...)
{
    return {};
}

template<u32 size> void emit_store(u32 rs, u32 rt, s16 imm)
{
    static_assert(std::has_single_bit(size));
    Gpq hs = get_gpr(rs), ht = get_gpr(rt);
    c.lea(eax, ptr(hs, imm));
    if constexpr (size > 1) {
        c.test(al, size - 1);
        c.jnz(l_early_block_epilogue);
    }
}

void add(u32 rs, u32 rt, u32 rd)
{
    Label l_noexception = c.newLabel();
    Gpd hs = get_gpr(rs).r32(), ht = get_gpr(rt).r32();
    c.mov(eax, rs);
    c.add(eax, ht);
    c.jno(l_noexception);
    block_epilog_with_pc_flush_and_jmp(integer_overflow_exception, 0);
    c.bind(l_noexception);
    if (rd) {
        Gpq hd = get_dirty_gpr(rd);
        c.movsxd(hd, eax);
    }
}

void addi(u32 rs, u32 rt, s16 imm)
{
    Label l_noexception = c.newLabel();
    Gpd hs = get_gpr(rs).r32();
    c.mov(eax, hs);
    c.add(eax, imm);
    c.jno(l_noexception);
    block_epilog_with_pc_flush_and_jmp(integer_overflow_exception, 0);
    c.bind(l_noexception);
    if (rt) {
        Gpq ht = get_dirty_gpr(rt);
        c.movsxd(ht, eax);
    }
}

void addiu(u32 rs, u32 rt, s16 imm)
{
    if (!rt) return;
    Gpd ht = get_dirty_gpr(rt).r32(), hs = get_gpr(rs).r32();
    c.lea(eax, ptr(hs, imm));
    c.movsxd(ht.r64(), eax);
}

void addu(u32 rs, u32 rt, u32 rd)
{
    if (!rt) return;
    Gpd hd = get_dirty_gpr(rd).r32(), hs = get_gpr(rs).r32(), ht = get_gpr(rt).r32();
    c.lea(eax, ptr(hs, ht));
    c.movsxd(hd.r64(), eax);
}

void and_(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void andi(u32, u32, u16)
{
}

void beq(u32, u32, s16)
{
}

void beql(u32, u32, s16)
{
}

void bgez(u32, s16)
{
}

void bgezal(u32, s16)
{
}

void bgezall(u32, s16)
{
}

void bgezl(u32, s16)
{
}

void bgtz(u32, s16)
{
}

void bgtzl(u32, s16)
{
}

void blez(u32, s16)
{
}

void blezl(u32, s16)
{
}

void bltz(u32, s16)
{
}

void bltzal(u32, s16)
{
}

void bltzall(u32, s16)
{
}

void bltzl(u32, s16)
{
}

void bne(u32, u32, s16)
{
}

void bnel(u32, u32, s16)
{
}

void break_()
{
}

void cache()
{
}

void dadd(u32 rs, u32 rt, u32 rd)
{
    Label l_noexception = c.newLabel();
    Gpq hs = get_gpr(rs), ht = get_gpr(rt);
    c.mov(rax, hs);
    c.add(rax, ht);
    c.jno(l_noexception);
    block_epilog_with_pc_flush_and_jmp(integer_overflow_exception, 0);
    c.bind(l_noexception);
    if (rd) {
        Gpq hd = get_dirty_gpr(rd);
        c.mov(hd, rax);
    }
}

void daddi(u32, u32, s16)
{
}

void daddiu(u32, u32, s16)
{
}

void daddu(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void div(u32 rs, u32 rt)
{
    Label l_div = c.newLabel(), l_divzero = c.newLabel(), l_end = c.newLabel();
    Gpd hs = get_gpr(rs).r32(), ht = get_gpr(rt).r32();
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.test(ht, ht);
    c.jz(l_divzero);
    c.mov(eax, hs);
    c.mov(edx, ht);
    c.btc(eax, 31);
    c.not_(edx);
    c.or_(edx, eax);
    c.jnz(l_div);
    c.movsxd(rax, hs);
    c.xor_(edx, edx);
    c.jmp(l_end);

    c.bind(l_divzero);
    c.movsxd(rax, hs);
    c.mov(edx, eax);
    c.sar(edx, 31);
    c.and_(edx, 2);
    c.dec(rdx);
    c.jmp(l_end);

    c.bind(l_div);
    c.mov(eax, hs);
    c.cdq(edx, eax);
    c.idiv(edx, eax, ht);
    c.cdqe(rax);
    c.movsxd(rdx, edx);

    c.bind(l_end);
    c.vmovq(hlo, rax);
    c.vmovq(hhi, rdx);
}

void div1(u32, u32)
{
}

void divu(u32 rs, u32 rt)
{
    Label l_div = c.newLabel(), l_end = c.newLabel();
    Gpd hs = get_gpr(rs).r32(), ht = get_gpr(rt).r32();
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.test(ht, ht);
    c.jnz(l_div);
    c.mov(rax, -1);
    c.movsxd(rdx, hs);
    c.jmp(l_end);

    c.bind(l_div);
    c.mov(eax, hs);
    c.xor_(edx, edx);
    c.div(edx, eax, ht);
    c.cdqe(rax);
    c.movsxd(rdx, edx);

    c.bind(l_end);
    c.vmovq(hlo, rax);
    c.vmovq(hhi, rdx);
}

void divu1(u32, u32)
{
}

void dsll(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsll32(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsllv(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsra(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsra32(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsrav(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsrl(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsrl32(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsrlv(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsub(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void dsubu(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void j(u32)
{
}

void jal(u32)
{
}

void jalr(u32, u32)
{
}

void jr(u32)
{
}

void lb(u32, u32, s16)
{
}

void lbu(u32, u32, s16)
{
}

void ld(u32, u32, s16)
{
}

void ldl(u32, u32, s16)
{
}

void ldr(u32, u32, s16)
{
}

void lh(u32, u32, s16)
{
}

void lhu(u32, u32, s16)
{
}

void lq(u32, u32, s16)
{
}

void lui(u32, s16)
{
}

void lw(u32, u32, s16)
{
}

void lwl(u32, u32, s16)
{
}

void lwr(u32, u32, s16)
{
}

void lwu(u32, u32, s16)
{
}

void madd(u32 rs, u32 rt, u32 rd) // Multiply/Add
{ // todo: rd == 0
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.movsxd(rax, hs.r32());
    c.movsxd(rcx, ht.r32());
    c.imul(rax, rcx);
    c.vmovd(ecx, hlo);
    c.add(rax, rcx);
    c.movsxd(hd, eax);
    c.vmovd(ecx, hhi);
    c.vmovq(hlo, hd);
    c.shl(rcx, 32);
    c.add(rax, rcx);
    c.sar(rax, 32);
    c.vmovq(hhi, rax);
}

void madd1(u32 rs, u32 rt, u32 rd) // Multiply-Add Pipeline 1
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.movsxd(rax, hs.r32());
    c.movsxd(rcx, ht.r32());
    c.imul(rax, rcx);
    c.vpextrd(ecx, hlo, 2);
    c.add(rax, rcx);
    c.movsxd(hd, eax);
    c.vpextrd(ecx, hhi, 2);
    c.vpinsrq(hlo, hlo, hd, 1);
    c.shl(rcx, 32);
    c.add(rax, rcx);
    c.sar(rax, 32);
    c.vpinsrq(hhi, hhi, rax, 1);
}

void maddu(u32 rs, u32 rt, u32 rd) // Multiply/Add Unsigned
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.mov(eax, hs.r32());
    c.mov(ecx, ht.r32());
    c.mul(rax, rcx);
    c.vmovd(ecx, hlo);
    c.add(rax, rcx);
    c.movsxd(hd, eax);
    c.vmovd(ecx, hhi);
    c.vmovq(hlo, hd);
    c.shl(rcx, 32);
    c.add(rax, rcx);
    c.sar(rax, 32);
    c.vmovq(hhi, rax);
}

void maddu1(u32 rs, u32 rt, u32 rd) // Multiply-Add Unsigned Pipeline 1
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.mov(eax, hs.r32());
    c.mov(ecx, ht.r32());
    c.mul(rax, rcx);
    c.vpextrd(ecx, hlo, 2);
    c.add(rax, rcx);
    c.movsxd(hd, eax);
    c.vpextrd(ecx, hhi, 2);
    c.vpinsrq(hlo, hlo, hd, 1);
    c.shl(rcx, 32);
    c.add(rax, rcx);
    c.sar(rax, 32);
    c.vpinsrq(hhi, hhi, rax, 1);
}

void mfhi(u32)
{
}

void mfhi1(u32)
{
}

void mflo(u32)
{
}

void mflo1(u32)
{
}

void mfsa(u32 rs)
{
    Gpq hs = get_dirty_gpr(rs);
    c.mov(hs.r32(), jit_ptr(sa));
}

void movn(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void movz(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void mthi(u32)
{
}

void mthi1(u32)
{
}

void mtlo(u32)
{
}

void mtlo1(u32)
{
}

void mtsa(u32 rs)
{
    Gpq hs = get_gpr(rs);
    c.mov(jit_ptr(sa), hs.r32());
}

void mtsab(u32 rs, s16 imm)
{
    Gpq hs = get_gpr(rs);
    c.lea(eax, ptr(hs, 0, 8u));
    c.and_(eax, 0x7f);
    c.xor_(eax, (imm & 15) << 3);
    c.mov(jit_ptr(sa), eax);
}

void mtsah(u32 rs, s16 imm)
{
    Gpq hs = get_gpr(rs);
    c.mov(eax, hs.r32());
    c.shl(eax, 16);
    c.and_(eax, 0x7f);
    c.xor_(eax, (imm & 7) << 4);
    c.mov(jit_ptr(sa), eax);
}

void mult(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.mov(eax, hs.r32());
    c.imul(eax, ht.r32());
    c.movsxd(hd, eax);
    c.movsxd(rdx, edx);
    c.vmovq(hlo, hd);
    c.vmovq(hhi, rdx);
}

void mult1(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.mov(eax, hs.r32());
    c.imul(eax, ht.r32());
    c.movsxd(hd, eax);
    c.movsxd(rdx, edx);
    c.vpinsrq(hlo, hlo, hd, 1);
    c.vpinsrq(hhi, hhi, rdx, 1);
}

void multu(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.mov(eax, hs.r32());
    c.mul(eax, ht.r32());
    c.movsxd(hd, eax);
    c.movsxd(rdx, edx);
    c.vmovq(hlo, hd);
    c.vmovq(hhi, rdx);
}

void multu1(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_dirty_lo(), hhi = get_dirty_hi();
    c.mov(eax, hs.r32());
    c.mul(eax, ht.r32());
    c.movsxd(hd, eax);
    c.movsxd(rdx, edx);
    c.vpinsrq(hlo, hlo, hd, 1);
    c.vpinsrq(hhi, hhi, rdx, 1);
}

void nor(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    if (rd != rs) {
        c.mov(hd, hs);
    }
    c.or_(hd, ht);
    c.not_(hd);
}

void or_(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    if (rd != rs) {
        c.mov(hd, hs);
    }
    c.or_(hd, ht);
}

void ori(u32, u32, u16)
{
}

void pref()
{
}

void sb(u32 rs, u32 rt, s16 imm)
{
    emit_store<1>(rs, rt, imm);
}

void sd(u32, u32, s16)
{
}

void sdl(u32, u32, s16)
{
}

void sdr(u32, u32, s16)
{
}

void sh(u32, u32, s16)
{
}

void sll(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void sllv(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void slt(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    c.cmp(hs, ht);
    c.setl(hd.r8());
    c.and_(hd.r32(), 1);
}

void slti(u32, u32, s16)
{
}

void sltiu(u32, u32, s16)
{
}

void sltu(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    c.cmp(hs, ht);
    c.setb(hd.r8());
    c.and_(hd.r32(), 1);
}

void sq(u32, u32, s16)
{
}

void sra(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void srav(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void srl(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void srlv(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void sub(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void subu(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void sw(u32, u32, s16)
{
}

void swl(u32, u32, s16)
{
}

void swr(u32, u32, s16)
{
}

void sync()
{
}

void syscall()
{
}

void teq(u32, u32)
{
}

void teqi(u32, s16)
{
}

void tge(u32, u32)
{
}

void tgei(u32, s16)
{
}

void tgeu(u32, u32)
{
}

void tgeiu(u32, s16)
{
}

void tlt(u32, u32)
{
}

void tlti(u32, s16)
{
}

void tltu(u32, u32)
{
}

void tltiu(u32, s16)
{
}

void tne(u32, u32)
{
}

void tnei(u32, s16)
{
}

void xor_(u32 rs, u32 rt, u32 rd)
{
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
}

void xori(u32, u32, u16)
{
}

} // namespace ee
