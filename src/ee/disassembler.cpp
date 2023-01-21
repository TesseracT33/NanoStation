#include "disassembler.hpp"
#include "cop0.hpp"
#include "cop1.hpp"
#include "cop2.hpp"
#include "cpu_interpreter.hpp"
#include "exceptions.hpp"

#define IMM16 (instr & 0xFFFF)
#define IMM26 (instr & 0x3FF'FFFF)
#define SA    (instr >> 6 & 31)
#define FD    (instr >> 6 & 31)
#define RD    (instr >> 11 & 31)
#define FS    (instr >> 11 & 31)
#define RT    (instr >> 16 & 31)
#define FT    (instr >> 16 & 31)
#define RS    (instr >> 21 & 31)
#define BASE  (instr >> 21 & 31)

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
    switch (instr >> 21 & 31) {
    case 0: mfc0(RD, RT); break;
    case 4: mtc0(RD, RT); break;

    case 8: { // BC0
        switch (instr >> 16 & 31) {
        case 0: bc0f(IMM16); break;
        case 1: bc0t(IMM16); break;
        case 2: bc0fl(IMM16); break;
        case 3: bc0tl(IMM16); break;
        default: reserved_instruction_exception();
        }
    } break;

    case 16: { // TLB
        switch (instr & 63) {
        case 0x01: tlbr(); break;
        case 0x02: tlbwi(); break;
        case 0x06: tlbwr(); break;
        case 0x08: tlbp(); break;
        case 0x18: eret(); break;
        case 0x38: ei(); break;
        case 0x39: di(); break;
        default: reserved_instruction_exception();
        }
    } break;

    default: reserved_instruction_exception();
    }
}

void cop1(u32 instr)
{
    switch (instr >> 21 & 31) {
    case 0x00: mfc1(FS, RT); break;
    case 0x02: cfc1(FS, RT); break;
    case 0x04: mtc1(FS, RT); break;
    case 0x06: ctc1(FS, RT); break;

    case 0x08: { // BC1
        switch (instr >> 16 & 31) {
        case 0: bc1f(IMM16); break;
        case 1: bc1t(IMM16); break;
        case 2: bc1fl(IMM16); break;
        case 3: bc1tl(IMM16); break;
        default: reserved_instruction_exception();
        }
    } break;

    case 0x10: { // FPU.S
        switch (instr & 63) {
        case 0x00: add_s(FD, FS, FT); break;
        case 0x01: sub_s(FD, FS, FT); break;
        case 0x02: mul_s(FD, FS, FT); break;
        case 0x03: div_s(FD, FS, FT); break;
        case 0x04: sqrt_s(FD, FS); break;
        case 0x05: abs_s(FD, FS); break;
        case 0x06: mov_s(FD, FS); break;
        case 0x07: neg_s(FD, FS); break;
        case 0x16: rsqrt_s(FD, FS); break;
        case 0x18: adda_s(FS, FT); break;
        case 0x19: suba_s(FS, FT); break;
        case 0x1A: mula_s(FS, FT); break;
        case 0x1C: madd_s(FD, FS, FT); break;
        case 0x1D: msub_s(FD, FS, FT); break;
        case 0x1E: madda_s(FS, FT); break;
        case 0x1F: msuba_s(FS, FT); break;
        case 0x24: cvt_w(FD, FS); break;
        case 0x28: max_s(FD, FS, FT); break;
        case 0x29: min_s(FD, FS, FT); break;
        case 0x30: c_f(FS, FT); break;
        case 0x32: c_eq(FS, FT); break;
        case 0x34: c_lt(FS, FT); break;
        case 0x36: c_le(FS, FT); break;
        default: reserved_instruction_exception();
        }
    } break;

    case 0x14: { // FPU.W
        if ((instr & 63) == 32) {
            cvt_s(FD, FS);
        } else {
            reserved_instruction_exception();
        }
    } break;

    default: reserved_instruction_exception();
    }
}

void cop2(u32 instr)
{
    u32 fmt = instr >> 21 & 31;
    if (fmt >= 16) { // Special1
        switch (instr & 63) {
        case 0x00: vaddx(); break;
        case 0x01: vaddy(); break;
        case 0x02: vaddz(); break;
        case 0x03: vaddw(); break;
        case 0x04: vsubx(); break;
        case 0x05: vsuby(); break;
        case 0x06: vsubz(); break;
        case 0x07: vsubw(); break;
        case 0x08: vmaddx(); break;
        case 0x09: vmaddy(); break;
        case 0x0A: vmaddz(); break;
        case 0x0B: vmaddw(); break;
        case 0x0C: vmsubx(); break;
        case 0x0D: vmsuby(); break;
        case 0x0E: vmsubz(); break;
        case 0x0F: vmsubw(); break;
        case 0x10: vmaxx(); break;
        case 0x11: vmaxy(); break;
        case 0x12: vmaxz(); break;
        case 0x13: vmaxw(); break;
        case 0x14: vminix(); break;
        case 0x15: vminiy(); break;
        case 0x16: vminiz(); break;
        case 0x17: vminiw(); break;
        case 0x18: vmulx(); break;
        case 0x19: vmuly(); break;
        case 0x1A: vmulz(); break;
        case 0x1B: vmulw(); break;
        case 0x1C: vmulq(); break;
        case 0x1D: vmaxi(); break;
        case 0x1E: vmuli(); break;
        case 0x1F: vminii(); break;
        case 0x20: vaddq(); break;
        case 0x21: vmaddq(); break;
        case 0x22: vaddi(); break;
        case 0x23: vmaddi(); break;
        case 0x24: vsubq(); break;
        case 0x25: vmsubq(); break;
        case 0x26: vsubi(); break;
        case 0x27: vmsubi(); break;
        case 0x28: vadd(); break;
        case 0x29: vmadd(); break;
        case 0x2A: vmul(); break;
        case 0x2B: vmax(); break;
        case 0x2C: vsub(); break;
        case 0x2D: vmsub(); break;
        case 0x2E: vopmsub(); break;
        case 0x2F: vmini(); break;
        case 0x30: viadd(); break;
        case 0x31: visub(); break;
        case 0x32: viaddi(); break;
        case 0x34: viand(); break;
        case 0x35: vior(); break;
        case 0x38: vcallms(); break;
        case 0x39: vcallmsr(); break;
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F: { // Special2
            if ((instr & 0x3C) == 0x3C) {
                switch (instr & 3 | (instr & 0xEC0) >> 4) {
                case 0x00: vaddax(); break;
                case 0x01: vadday(); break;
                case 0x02: vaddaz(); break;
                case 0x03: vaddaw(); break;
                case 0x04: vsubax(); break;
                case 0x05: vsubay(); break;
                case 0x06: vsubaz(); break;
                case 0x07: vsubaw(); break;
                case 0x08: vmaddax(); break;
                case 0x09: vmadday(); break;
                case 0x0A: vmaddaz(); break;
                case 0x0B: vmaddaw(); break;
                case 0x0C: vmsubax(); break;
                case 0x0D: vmsubay(); break;
                case 0x0E: vmsubaz(); break;
                case 0x0F: vmsubaw(); break;
                case 0x10: vitof0(); break;
                case 0x11: vitof4(); break;
                case 0x12: vitof12(); break;
                case 0x13: vitof15(); break;
                case 0x14: vftoi0(); break;
                case 0x15: vftoi4(); break;
                case 0x16: vftoi12(); break;
                case 0x17: vftoi15(); break;
                case 0x18: vmulax(); break;
                case 0x19: vmulay(); break;
                case 0x1A: vmulaz(); break;
                case 0x1B: vmulaw(); break;
                case 0x1C: vmulaq(); break;
                case 0x1D: vabs(); break;
                case 0x1E: vmulai(); break;
                case 0x1F: vclipw(); break;
                case 0x20: vaddaq(); break;
                case 0x21: vmaddaq(); break;
                case 0x22: vaddai(); break;
                case 0x23: vmaddai(); break;
                case 0x24: vsubaq(); break;
                case 0x25: vmsubaq(); break;
                case 0x26: vsubai(); break;
                case 0x27: vmsubai(); break;
                case 0x28: vadda(); break;
                case 0x29: vmadda(); break;
                case 0x2A: vmula(); break;
                case 0x2C: vsuba(); break;
                case 0x2D: vmsuba(); break;
                case 0x2E: vopmula(); break;
                case 0x2F: vnop(); break;
                case 0x30: vmove(); break;
                case 0x31: vmr32(); break;
                case 0x34: vlqi(); break;
                case 0x35: vsqi(); break;
                case 0x36: vlqd(); break;
                case 0x37: vsqd(); break;
                case 0x38: vdiv(); break;
                case 0x39: vsqrt(); break;
                case 0x3A: vrsqrt(); break;
                case 0x3B: vwaitq(); break;
                case 0x3C: vmtir(); break;
                case 0x3D: vmfir(); break;
                case 0x3E: vilwr(); break;
                case 0x3F: viswr(); break;
                case 0x40: vrnext(); break;
                case 0x41: vrget(); break;
                case 0x42: vrinit(); break;
                case 0x43: vrxor(); break;

                default: reserved_instruction_exception();
                }
            } else {
                reserved_instruction_exception();
            }
        } break;

        default: reserved_instruction_exception();
        }
    } else {
        switch (fmt) {
        case 1: qmfc2(); break;
        case 2: cfc2(); break;
        case 5: qmtc2(); break;
        case 6: ctc2(); break;
        case 8: { // BC2
            switch (instr >> 16 & 31) {
            case 0: bc2f(); break;
            case 1: bc2t(); break;
            case 2: bc2fl(); break;
            case 3: bc2tl(); break;
            default: reserved_instruction_exception();
            }
        } break;
        default: reserved_instruction_exception();
        }
    }
}

void cop3(u32 instr)
{
}

void mmi(u32 instr)
{
    auto mmi0 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        // case 0x00: paddw(); break;
        // case 0x01: psubw(); break;
        // case 0x02: pcgtw(); break;
        // case 0x03: pmaxw(); break;
        // case 0x04: paddh(); break;
        // case 0x05: psubh(); break;
        // case 0x06: pcgth(); break;
        // case 0x07: pmaxh(); break;
        // case 0x08: paddb(); break;
        // case 0x09: psubb(); break;
        // case 0x0A: pcgtb(); break;
        // case 0x10: paddsw(); break;
        // case 0x11: psubsw(); break;
        // case 0x12: pextlw(); break;
        // case 0x13: ppacw(); break;
        // case 0x14: paddsh(); break;
        // case 0x15: psubsh(); break;
        // case 0x16: pextlh(); break;
        // case 0x17: ppach(); break;
        // case 0x18: paddsb(); break;
        // case 0x19: psubsb(); break;
        // case 0x1A: pextlb(); break;
        // case 0x1B: ppacb(); break;
        // case 0x1E: pext5(); break;
        // case 0x1F: ppac5(); break;
        default: reserved_instruction_exception();
        }
    };

    auto mmi1 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        // case 0x01: pabsw(); break;
        // case 0x02: pceqw(); break;
        // case 0x03: pminw(); break;
        // case 0x04: padsbh(); break;
        // case 0x05: pabsh(); break;
        // case 0x06: pceqh(); break;
        // case 0x07: pminh(); break;
        // case 0x0A: pceqb(); break;
        // case 0x10: padduw(); break;
        // case 0x11: psubuw(); break;
        // case 0x12: pextuw(); break;
        // case 0x14: padduh(); break;
        // case 0x15: psubuh(); break;
        // case 0x16: pextuh(); break;
        // case 0x18: paddub(); break;
        // case 0x19: psubub(); break;
        // case 0x1A: pextub(); break;
        // case 0x1B: qfsrv(); break;
        default: reserved_instruction_exception();
        }
    };

    auto mmi2 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        // case 0x00: pmaddw(); break;
        // case 0x02: psllvw(); break;
        // case 0x03: psrlvw(); break;
        // case 0x04: pmsubw(); break;
        // case 0x08: pmfhi(); break;
        // case 0x09: pmflo(); break;
        // case 0x0A: pinth(); break;
        // case 0x0C: pmultw(); break;
        // case 0x0D: pdivw(); break;
        // case 0x0E: pcpyld(); break;
        // case 0x10: pmaddh(); break;
        // case 0x11: phmadh(); break;
        // case 0x12: pand(); break;
        // case 0x13: pxor(); break;
        // case 0x14: pmsubh(); break;
        // case 0x15: phmsbh(); break;
        // case 0x1A: pexeh(); break;
        // case 0x1B: prevh(); break;
        // case 0x1C: pmulth(); break;
        // case 0x1D: pdivbw(); break;
        // case 0x1E: pexew(); break;
        // case 0x1F: prot3w(); break;
        default: reserved_instruction_exception();
        }
    };

    auto mmi3 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        // case 0x00: pmadduw(); break;
        // case 0x03: psravw(); break;
        // case 0x08: pmthi(); break;
        // case 0x09: pmtlo(); break;
        // case 0x0A: pinteh(); break;
        // case 0x0C: pmultuw(); break;
        // case 0x0D: pdivuw(); break;
        // case 0x0E: pcpyud(); break;
        // case 0x12: por(); break;
        // case 0x13: pnor(); break;
        // case 0x1A: pexch(); break;
        // case 0x1B: pcpyh(); break;
        // case 0x1E: pexcw(); break;
        default: reserved_instruction_exception();
        }
    };

    switch (instr & 63) {
    // case 0x00: madd(); break;
    // case 0x01: maddu(); break;
    // case 0x04: plzcw(); break;
    case 0x08: mmi0(instr); break;
    case 0x09: mmi2(instr); break;
    case 0x10: mfhi1(RD); break;
    case 0x11: mthi1(RS); break;
    case 0x12: mflo1(RD); break;
    case 0x13: mtlo1(RS); break;
    case 0x18: mult1(RS, RT); break;
    case 0x19: multu1(RS, RT); break;
    case 0x1A: div1(RS, RT); break;
    case 0x1B: divu1(RS, RT); break;
    // case 0x20: madd1(); break;
    // case 0x21: maddu1(); break;
    case 0x28: mmi1(instr); break;
    case 0x29: mmi3(instr); break;
    // case 0x30: pmfhl(); break;
    // case 0x31: pmthl(); break;
    // case 0x34: psllh(); break;
    // case 0x36: psrlh(); break;
    // case 0x37: psrah(); break;
    // case 0x3C: psllw(); break;
    // case 0x3E: psrlw(); break;
    // case 0x3F: psraw(); break;
    default: reserved_instruction_exception();
    }
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
    case 0x18: mtsab(); break;
    case 0x19: mtsah(); break;
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
    case 0x28: mfsa(RD); break;
    case 0x29: mtsa(RS); break;
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
    case 0x31: lwc1(FT, BASE, IMM16); break;
    case 0x33: pref(); break;
    case 0x36: lqc2(); break;
    case 0x37: ld(RS, RT, IMM16); break;
    case 0x39: swc1(FT, BASE, IMM16); break;
    case 0x3E: sqc2(); break;
    case 0x3F: sd(RS, RT, IMM16); break;
    default: reserved_instruction_exception();
    }
}
} // namespace ee::interpreter
