#include "disassembler.hpp"
#include "cpu_interpreter.hpp"
#include "exceptions.hpp"

#define IMM16 (instr & 0xFFFF)
#define IMM26 (instr & 0x3FF'FFFF)
#define SA    (instr >> 6 & 31)
#define RD    (instr >> 11 & 31)
#define RT    (instr >> 16 & 31)
#define RS    (instr >> 21 & 31)

namespace ee::interpreter {
static void cop0(u32 instr);
static void cop1(u32 instr);
static void cop2(u32 instr);
static void cop3(u32 instr);
static void mmi(u32 instr);
static void regimm(u32 instr);
static void special(u32 instr);

void cop0(u32 instr)
{
}

void cop1(u32 instr)
{
}

void cop2(u32 instr)
{
}

void cop3(u32 instr)
{
}

void mmi(u32 instr)
{
}

void regimm(u32 instr)
{
    switch (instr >> 16 & 31) {
    case 0x00: bltz(RS, IMM16); break;
    case 0x01: bgez(RS, IMM16); break;
    case 0x02: bltzl(RS, IMM16); break;
    case 0x03: bgezl(RS, IMM16); break;
    case 0x08: tgei(RS, IMM16); break;
    case 0x09: tgeiu(RS, IMM16); break;
    case 0x0A: tlti(RS, IMM16); break;
    case 0x0B: tltiu(RS, IMM16); break;
    case 0x0C: teqi(RS, IMM16); break;
    case 0x0E: tnei(RS, IMM16); break;
    case 0x10: bltzal(RS, IMM16); break;
    case 0x11: bgezal(RS, IMM16); break;
    case 0x12: bltzall(RS, IMM16); break;
    case 0x13: bgezall(RS, IMM16); break;
    // case 0x18: mtsab(); break;
    // case 0x19: mtsah(); break;
    default: reserved_instruction_exception();
    }
}

void special(u32 instr)
{
    switch (instr & 63) {
    case 0x00: sll(RT, RD, SA); break;
    case 0x02: srl(RT, RD, SA); break;
    case 0x03: sra(RT, RD, SA); break;
    case 0x04: sllv(RS, RT, RD); break;
    case 0x06: srlv(RS, RT, RD); break;
    case 0x07: srav(RS, RT, RD); break;
    case 0x08: jr(RS); break;
    case 0x09: jalr(RS, RD); break;
    case 0x0A: movz(RS, RT, RD); break;
    case 0x0B: movn(RS, RT, RD); break;
    case 0x0C: syscall(); break;
    case 0x0D: break_(); break;
    case 0x0F: sync(); break;
    case 0x10: mfhi(RD); break;
    case 0x11: mthi(RS); break;
    case 0x12: mflo(RD); break;
    case 0x13: mtlo(RS); break;
    case 0x14: dsllv(RS, RT, RD); break;
    case 0x16: dsrlv(RS, RT, RD); break;
    case 0x17: dsrav(RS, RT, RD); break;
    case 0x18: mult(RS, RT); break;
    case 0x19: multu(RS, RT); break;
    case 0x1A: div(RS, RT); break;
    case 0x1B: divu(RS, RT); break;
    case 0x20: add(RS, RT, RD); break;
    case 0x21: addu(RS, RT, RD); break;
    case 0x22: sub(RS, RT, RD); break;
    case 0x23: subu(RS, RT, RD); break;
    case 0x24: and_(RS, RT, RD); break;
    case 0x25: or_(RS, RT, RD); break;
    case 0x26: xor_(RS, RT, RD); break;
    case 0x27: nor(RS, RT, RD); break;
    // case 0x28: mfsa(); break;
    // case 0x29: mtsa(); break;
    case 0x2A: slt(RS, RT, RD); break;
    case 0x2B: sltu(RS, RT, RD); break;
    case 0x2C: dadd(RS, RT, RD); break;
    case 0x2D: daddu(RS, RT, RD); break;
    case 0x2E: dsub(RS, RT, RD); break;
    case 0x2F: dsubu(RS, RT, RD); break;
    case 0x30: tge(RS, RT); break;
    case 0x31: tgeu(RS, RT); break;
    case 0x32: tlt(RS, RT); break;
    case 0x33: tltu(RS, RT); break;
    case 0x34: teq(RS, RT); break;
    case 0x36: tne(RS, RT); break;
    case 0x38: dsll(RT, RD, SA); break;
    case 0x3A: dsrl(RT, RD, SA); break;
    case 0x3B: dsra(RT, RD, SA); break;
    case 0x3C: dsll32(RT, RD, SA); break;
    case 0x3E: dsrl32(RT, RD, SA); break;
    case 0x3F: dsra32(RT, RD, SA); break;
    default: reserved_instruction_exception();
    }
}

void disassemble(u32 instr)
{
    switch (instr >> 26 & 63) {
    case 0x00: special(instr); break;
    case 0x01: regimm(instr); break;
    case 0x02: j(IMM26); break;
    case 0x03: jal(IMM26); break;
    case 0x04: beq(RS, RT, IMM16); break;
    case 0x05: bne(RS, RT, IMM16); break;
    case 0x06: blez(RS, IMM16); break;
    case 0x07: bgtz(RS, IMM16); break;
    case 0x08: addi(RS, RT, IMM16); break;
    case 0x09: addiu(RS, RT, IMM16); break;
    case 0x0A: slti(RS, RT, IMM16); break;
    case 0x0B: sltiu(RS, RT, IMM16); break;
    case 0x0C: andi(RS, RT, IMM16); break;
    case 0x0D: ori(RS, RT, IMM16); break;
    case 0x0E: xori(RS, RT, IMM16); break;
    case 0x0F: lui(RT, IMM16); break;
    case 0x10: cop0(instr); break;
    case 0x11: cop1(instr); break;
    case 0x12: cop2(instr); break;
    case 0x13: cop3(instr); break;
    case 0x14: beql(RS, RT, IMM16); break;
    case 0x15: bnel(RS, RT, IMM16); break;
    case 0x16: blezl(RS, IMM16); break;
    case 0x17: bgtzl(RS, IMM16); break;
    case 0x18: daddi(RS, RT, IMM16); break;
    case 0x19: daddiu(RS, RT, IMM16); break;
    case 0x1A: ldl(RS, RT, IMM16); break;
    case 0x1B: ldr(RS, RT, IMM16); break;
    case 0x1C: mmi(instr); break;
    case 0x1E: lq(RS, RT, IMM16); break;
    case 0x1F: sq(RS, RT, IMM16); break;
    case 0x20: lb(RS, RT, IMM16); break;
    case 0x21: lh(RS, RT, IMM16); break;
    case 0x22: lwl(RS, RT, IMM16); break;
    case 0x23: lw(RS, RT, IMM16); break;
    case 0x24: lbu(RS, RT, IMM16); break;
    case 0x25: lhu(RS, RT, IMM16); break;
    case 0x26: lwr(RS, RT, IMM16); break;
    case 0x27: lwu(RS, RT, IMM16); break;
    case 0x28: sb(RS, RT, IMM16); break;
    case 0x29: sh(RS, RT, IMM16); break;
    case 0x2A: swl(RS, RT, IMM16); break;
    case 0x2B: sw(RS, RT, IMM16); break;
    case 0x2C: sdl(RS, RT, IMM16); break;
    case 0x2D: sdr(RS, RT, IMM16); break;
    case 0x2E: swr(RS, RT, IMM16); break;
    case 0x2F: cache(); break;
    // case 0x31: lwc1(); break;
    case 0x33: pref(); break;
    // case 0x36: lqc2(); break;
    case 0x37: ld(RS, RT, IMM16); break;
    // case 0x39: swc1(); break;
    // case 0x3E: sqc2(); break;
    case 0x3F: sd(RS, RT, IMM16); break;
    default: reserved_instruction_exception();
    }
}
} // namespace ee::interpreter
