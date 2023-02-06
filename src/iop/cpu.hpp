#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

namespace iop {
template<mips::CpuImpl> void add(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void addi(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void addiu(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void addu(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void and_(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void andi(u32 rs, u32 rt, u16 imm);
template<mips::CpuImpl> void beq(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void bgez(u32 rs, s16 imm);
template<mips::CpuImpl> void bgezal(u32 rs, s16 imm);
template<mips::CpuImpl> void bgtz(u32 rs, s16 imm);
template<mips::CpuImpl> void blez(u32 rs, s16 imm);
template<mips::CpuImpl> void bltz(u32 rs, s16 imm);
template<mips::CpuImpl> void bltzal(u32 rs, s16 imm);
template<mips::CpuImpl> void bne(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void break_();
template<mips::CpuImpl> void div(u32 rs, u32 rt);
template<mips::CpuImpl> void divu(u32 rs, u32 rt);
template<mips::CpuImpl> void j(u32 imm26);
template<mips::CpuImpl> void jal(u32 imm26);
template<mips::CpuImpl> void jalr(u32 rs, u32 rd);
template<mips::CpuImpl> void jr(u32 rs);
template<mips::CpuImpl> void lb(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lbu(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lh(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lhu(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lui(u32 rt, s16 imm);
template<mips::CpuImpl> void lw(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lwl(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lwr(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void lwu(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void mfhi(u32 rd);
template<mips::CpuImpl> void mflo(u32 rd);
template<mips::CpuImpl> void mthi(u32 rs);
template<mips::CpuImpl> void mtlo(u32 rs);
template<mips::CpuImpl> void mult(u32 rs, u32 rt);
template<mips::CpuImpl> void multu(u32 rs, u32 rt);
template<mips::CpuImpl> void nor(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void or_(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void ori(u32 rs, u32 rt, u16 imm);
template<mips::CpuImpl> void sb(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void sh(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void sll(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void sllv(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void slt(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void slti(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void sltiu(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void sltu(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void sra(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void srav(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void srl(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void srlv(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void sub(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void subu(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void sw(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void swl(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void swr(u32 rs, u32 rt, s16 imm);
template<mips::CpuImpl> void syscall();
template<mips::CpuImpl> void xor_(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void xori(u32 rs, u32 rt, u16 imm);
} // namespace iop
