#pragma once

#include "numtypes.hpp"

namespace iop {

void add(u32 rs, u32 rt, u32 rd);
void addi(u32 rs, u32 rt, s16 imm);
void addiu(u32 rs, u32 rt, s16 imm);
void addu(u32 rs, u32 rt, u32 rd);
void and_(u32 rs, u32 rt, u32 rd);
void andi(u32 rs, u32 rt, u16 imm);
void beq(u32 rs, u32 rt, s16 imm);
void bgez(u32 rs, s16 imm);
void bgezal(u32 rs, s16 imm);
void bgtz(u32 rs, s16 imm);
void blez(u32 rs, s16 imm);
void bltz(u32 rs, s16 imm);
void bltzal(u32 rs, s16 imm);
void bne(u32 rs, u32 rt, s16 imm);
void break_();
void div(u32 rs, u32 rt);
void divu(u32 rs, u32 rt);
void j(u32 imm26);
void jal(u32 imm26);
void jalr(u32 rs, u32 rd);
void jr(u32 rs);
void lb(u32 rs, u32 rt, s16 imm);
void lbu(u32 rs, u32 rt, s16 imm);
void lh(u32 rs, u32 rt, s16 imm);
void lhu(u32 rs, u32 rt, s16 imm);
void lui(u32 rt, s16 imm);
void lw(u32 rs, u32 rt, s16 imm);
void lwl(u32 rs, u32 rt, s16 imm);
void lwr(u32 rs, u32 rt, s16 imm);
void lwu(u32 rs, u32 rt, s16 imm);
void mfhi(u32 rd);
void mflo(u32 rd);
void mthi(u32 rs);
void mtlo(u32 rs);
void mult(u32 rs, u32 rt, u32 rd);
void multu(u32 rs, u32 rt, u32 rd);
void nor(u32 rs, u32 rt, u32 rd);
void or_(u32 rs, u32 rt, u32 rd);
void ori(u32 rs, u32 rt, u16 imm);
void sb(u32 rs, u32 rt, s16 imm);
void sh(u32 rs, u32 rt, s16 imm);
void sll(u32 rt, u32 rd, u32 sa);
void sllv(u32 rs, u32 rt, u32 rd);
void slt(u32 rs, u32 rt, u32 rd);
void slti(u32 rs, u32 rt, s16 imm);
void sltiu(u32 rs, u32 rt, s16 imm);
void sltu(u32 rs, u32 rt, u32 rd);
void sra(u32 rt, u32 rd, u32 sa);
void srav(u32 rs, u32 rt, u32 rd);
void srl(u32 rt, u32 rd, u32 sa);
void srlv(u32 rs, u32 rt, u32 rd);
void sub(u32 rs, u32 rt, u32 rd);
void subu(u32 rs, u32 rt, u32 rd);
void sw(u32 rs, u32 rt, s16 imm);
void swl(u32 rs, u32 rt, s16 imm);
void swr(u32 rs, u32 rt, s16 imm);
void syscall();
void xor_(u32 rs, u32 rt, u32 rd);
void xori(u32 rs, u32 rt, u16 imm);

} // namespace iop
