#include "disassembler.hpp"

#define IMM16 (instr & 0xFFFF)
#define IMM26 (instr & 0x3FF'FFFF)
#define SA    (instr >> 6 & 31)
#define RD    (instr >> 11 & 31)
#define RT    (instr >> 16 & 31)
#define RS    (instr >> 21 & 31)

namespace {
void cop0(u32 instr);
void cop1(u32 instr);
void cop2(u32 instr);
void cop3(u32 instr);
void mmi(u32 instr);
void regimm(u32 instr);
void special(u32 instr);
} // namespace

template<CpuImpl cpu_impl> void ee::disassemble(u32 instr)
{
    switch (instr >> 26 & 63) {
    case 0x00: special(instr); break;
    case 0x01: regimm(instr); break;
    case 0x02: break;
    case 0x03: break;
    case 0x04: break;
    case 0x05: break;
    case 0x06: break;
    case 0x07: break;
    case 0x08: break;
    case 0x09: break;
    case 0x0A: break;
    case 0x0B: break;
    case 0x0C: break;
    case 0x0D: break;
    case 0x0E: break;
    case 0x0F: break;
    case 0x10: cop0(instr); break;
    case 0x11: cop1(instr); break;
    case 0x12: cop2(instr); break;
    case 0x13: cop3(instr); break;
    case 0x14: break;
    case 0x15: break;
    case 0x16: break;
    case 0x17: break;
    case 0x18: break;
    case 0x19: break;
    case 0x1A: break;
    case 0x1B: break;
    case 0x1C: break;
    case 0x1D: break;
    case 0x1E: break;
    case 0x1F: break;
    default: break;
    }
}
