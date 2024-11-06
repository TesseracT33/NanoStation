#include "cpu.hpp"
#include "ee.hpp"
#include "exceptions.hpp"
#include "mmu.hpp"

namespace ee {

void add(u32, u32, u32)
{
}

void addi(u32, u32, s16)
{
}

void addiu(u32, u32, s16)
{
}

void addu(u32, u32, u32)
{
}

void and_(u32, u32, u32)
{
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

void dadd(u32, u32, u32)
{
}

void daddi(u32, u32, s16)
{
}

void daddiu(u32, u32, s16)
{
}

void daddu(u32, u32, u32)
{
}

void div(u32, u32)
{
}

void div1(u32, u32)
{
}

void divu(u32, u32)
{
}

void divu1(u32, u32)
{
}

void dsll(u32, u32, u32)
{
}

void dsll32(u32, u32, u32)
{
}

void dsllv(u32, u32, u32)
{
}

void dsra(u32, u32, u32)
{
}

void dsra32(u32, u32, u32)
{
}

void dsrav(u32, u32, u32)
{
}

void dsrl(u32, u32, u32)
{
}

void dsrl32(u32, u32, u32)
{
}

void dsrlv(u32, u32, u32)
{
}

void dsub(u32, u32, u32)
{
}

void dsubu(u32, u32, u32)
{
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
{
    /*
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_lo(), hhi = get_hi();
    c.movsxd(rax, hs.r32());
    c.movsxd(rcx, ht.r32());
    c.imul(rax, rcx);
    c.vmovd(ecx, hlo);
    c.movsxd(rcx, ecx);
    c.add(rax, rcx);
    c.movsxd(hd, eax);
    c.vmovd(ecx, hhi);
    c.vmovq(hlo, hd);
    c.shl(rcx, 32);
    c.add(rax, rcx);
    c.sar(rax, 32);
    c.vmovq(hhi, rax);
    */
    (void)rs;
    (void)rt;
    (void)rd;
}

void madd1(u32 rs, u32 rt, u32 rd) // Multiply-Add Pipeline 1
{
    /*
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_lo(), hhi = get_hi();
    c.movsxd(rax, hs.r32());
    c.movsxd(rcx, ht.r32());
    c.imul(rax, rcx);
    c.vpextrd(ecx, hlo, 2);
    c.movsxd(rcx, ecx);
    c.add(rax, rcx);
    c.movsxd(hd, eax);
    c.vpextrd(ecx, hhi, 2);
    c.vpinsrq(hlo, hlo, hd, 1);
    c.shl(rcx, 32);
    c.add(rax, rcx);
    c.sar(rax, 32);
    c.vpinsrq(hhi, hhi, rax, 1);
    */
    (void)rs;
    (void)rt;
    (void)rd;
}

void maddu(u32 rs, u32 rt, u32 rd) // Multiply/Add Unsigned
{
    /*
    Gpq hd = get_dirty_gpr(rd), hs = get_gpr(rs), ht = get_gpr(rt);
    Xmm hlo = get_lo(), hhi = get_hi();
    c.mov(eax, hs.r32());
    c.mov(ecx, ht.r32());
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
    */
    (void)rs;
    (void)rt;
    (void)rd;
}

void maddu1(u32 rs, u32 rt, u32 rd) // Multiply-Add Unsigned Pipeline 1
{
    (void)rs;
    (void)rt;
    (void)rd;
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

void mfsa(u32)
{
}

void movn(u32, u32, u32)
{
}

void movz(u32, u32, u32)
{
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

void mtsa(u32)
{
}

void mtsab(u32, s16)
{
}

void mtsah(u32, s16)
{
}

void mult(u32, u32)
{
}

void mult1(u32, u32)
{
}

void multu(u32, u32)
{
}

void multu1(u32, u32)
{
}

void nor(u32, u32, u32)
{
}

void or_(u32, u32, u32)
{
}

void ori(u32, u32, u16)
{
}

void pref()
{
}

void sb(u32, u32, s16)
{
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

void sll(u32, u32, u32)
{
}

void sllv(u32, u32, u32)
{
}

void slt(u32, u32, u32)
{
}

void slti(u32, u32, s16)
{
}

void sltiu(u32, u32, s16)
{
}

void sltu(u32, u32, u32)
{
}

void sq(u32, u32, s16)
{
}

void sra(u32, u32, u32)
{
}

void srav(u32, u32, u32)
{
}

void srl(u32, u32, u32)
{
}

void srlv(u32, u32, u32)
{
}

void sub(u32, u32, u32)
{
}

void subu(u32, u32, u32)
{
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

void xor_(u32, u32, u32)
{
}

void xori(u32, u32, u16)
{
}

} // namespace ee
