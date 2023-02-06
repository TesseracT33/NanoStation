#include "disassembler.hpp"
#include "ee/cop0.hpp"
#include "ee/cop1.hpp"
#include "ee/cop2.hpp"
#include "ee/cpu.hpp"
#include "ee/exceptions.hpp"
#include "ee/mmi.hpp"
#include "iop/cop0.hpp"
#include "iop/cop2.hpp"
#include "iop/cpu.hpp"
#include "iop/exceptions.hpp"
#include "mips.hpp"

#define IMM16 (instr & 0xFFFF)
#define IMM26 (instr & 0x3FF'FFFF)
#define FMT   (instr & 63)
#define SA    (instr >> 6 & 31)
#define FD    (instr >> 6 & 31)
#define RD    (instr >> 11 & 31)
#define FS    (instr >> 11 & 31)
#define RT    (instr >> 16 & 31)
#define FT    (instr >> 16 & 31)
#define RS    (instr >> 21 & 31)
#define BASE  (instr >> 21 & 31)

namespace mips {

enum class Cpu {
    EE,
    IOP
};

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void cop0(u32 instr);
template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void cop1(u32 instr);
template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void cop2(u32 instr);
template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void cop3(u32 instr);
template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void mmi(u32 instr);
template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void regimm(u32 instr);
template<Cpu cpu, bool make_string> static void reserved_instruction(auto instr);
template<Cpu cpu, CpuImpl cpu_impl, bool make_string> static void special(u32 instr);

static std::string disassemble_result;

#define INSTR_COMMON(instr_name, ...)                                        \
    {                                                                        \
        if constexpr (cpu == Cpu::EE) ee::instr_name<cpu_impl>(__VA_ARGS__); \
        else iop::instr_name<cpu_impl>(__VA_ARGS__);                         \
    } // namespace mips

#define INSTR_EE(instr_name, ...)                                            \
    {                                                                        \
        if constexpr (cpu == Cpu::EE) ee::instr_name<cpu_impl>(__VA_ARGS__); \
        else reserved_instruction<Cpu::IOP, make_string>(#instr_name);       \
    } // namespace mips

#define INSTR_IOP(instr_name, ...)                                                             \
    {                                                                                          \
        if constexpr (cpu == Cpu::EE) reserved_instruction<Cpu::EE, make_string>(#instr_name); \
        else iop::instr_name<cpu_impl>(__VA_ARGS__)                                            \
    } // namespace mips

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void cop0(u32 instr)
{
    switch (instr >> 21 & 31) {
    case 0: INSTR_COMMON(mfc0, RD, RT); break;
    case 4: INSTR_COMMON(mtc0, RD, RT); break;

    case 8: { // BC0
        switch (instr >> 16 & 31) {
        case 0: INSTR_EE(bc0f, IMM16); break;
        case 1: INSTR_EE(bc0t, IMM16); break;
        case 2: INSTR_EE(bc0fl, IMM16); break;
        case 3: INSTR_EE(bc0tl, IMM16); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    } break;

    case 16: { // TLB
        switch (instr & 63) {
        case 0x01: INSTR_EE(tlbr); break;
        case 0x02: INSTR_EE(tlbwi); break;
        case 0x06: INSTR_EE(tlbwr); break;
        case 0x08: INSTR_EE(tlbp); break;
        case 0x18: INSTR_EE(eret); break;
        case 0x38: INSTR_EE(ei); break;
        case 0x39: INSTR_EE(di); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    } break;

    default: reserved_instruction<cpu, make_string>(instr);
    }
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void cop1(u32 instr)
{
    if constexpr (cpu == Cpu::IOP) {
        reserved_instruction<Cpu::IOP, make_string>(instr);
        return;
    }

    switch (instr >> 21 & 31) {
    case 0x00: INSTR_EE(mfc1, FS, RT); break;
    case 0x02: INSTR_EE(cfc1, FS, RT); break;
    case 0x04: INSTR_EE(mtc1, FS, RT); break;
    case 0x06: INSTR_EE(ctc1, FS, RT); break;

    case 0x08: { // BC1
        switch (instr >> 16 & 31) {
        case 0: INSTR_EE(bc1f, IMM16); break;
        case 1: INSTR_EE(bc1t, IMM16); break;
        case 2: INSTR_EE(bc1fl, IMM16); break;
        case 3: INSTR_EE(bc1tl, IMM16); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    } break;

    case 0x10: { // FPU.S
        switch (instr & 63) {
        case 0x00: INSTR_EE(add_s, FD, FS, FT); break;
        case 0x01: INSTR_EE(sub_s, FD, FS, FT); break;
        case 0x02: INSTR_EE(mul_s, FD, FS, FT); break;
        case 0x03: INSTR_EE(div_s, FD, FS, FT); break;
        case 0x04: INSTR_EE(sqrt_s, FD, FS); break;
        case 0x05: INSTR_EE(abs_s, FD, FS); break;
        case 0x06: INSTR_EE(mov_s, FD, FS); break;
        case 0x07: INSTR_EE(neg_s, FD, FS); break;
        case 0x16: INSTR_EE(rsqrt_s, FD, FS); break;
        case 0x18: INSTR_EE(adda_s, FS, FT); break;
        case 0x19: INSTR_EE(suba_s, FS, FT); break;
        case 0x1A: INSTR_EE(mula_s, FS, FT); break;
        case 0x1C: INSTR_EE(madd_s, FD, FS, FT); break;
        case 0x1D: INSTR_EE(msub_s, FD, FS, FT); break;
        case 0x1E: INSTR_EE(madda_s, FS, FT); break;
        case 0x1F: INSTR_EE(msuba_s, FS, FT); break;
        case 0x24: INSTR_EE(cvt_w, FD, FS); break;
        case 0x28: INSTR_EE(max_s, FD, FS, FT); break;
        case 0x29: INSTR_EE(min_s, FD, FS, FT); break;
        case 0x30: INSTR_EE(c_f, FS, FT); break;
        case 0x32: INSTR_EE(c_eq, FS, FT); break;
        case 0x34: INSTR_EE(c_lt, FS, FT); break;
        case 0x36: INSTR_EE(c_le, FS, FT); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    } break;

    case 0x14: { // FPU.W
        if ((instr & 63) == 32) {
            INSTR_EE(cvt_s, FD, FS);
        } else {
            reserved_instruction<cpu, make_string>(instr);
        }
    } break;

    default: reserved_instruction<cpu, make_string>(instr);
    }
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void cop2(u32 instr)
{
    if constexpr (cpu == Cpu::EE) {
        u32 fmt = instr >> 21 & 31;
        if (fmt >= 16) { // Special1
            switch (instr & 63) {
            case 0x00: INSTR_EE(vaddx); break;
            case 0x01: INSTR_EE(vaddy); break;
            case 0x02: INSTR_EE(vaddz); break;
            case 0x03: INSTR_EE(vaddw); break;
            case 0x04: INSTR_EE(vsubx); break;
            case 0x05: INSTR_EE(vsuby); break;
            case 0x06: INSTR_EE(vsubz); break;
            case 0x07: INSTR_EE(vsubw); break;
            case 0x08: INSTR_EE(vmaddx); break;
            case 0x09: INSTR_EE(vmaddy); break;
            case 0x0A: INSTR_EE(vmaddz); break;
            case 0x0B: INSTR_EE(vmaddw); break;
            case 0x0C: INSTR_EE(vmsubx); break;
            case 0x0D: INSTR_EE(vmsuby); break;
            case 0x0E: INSTR_EE(vmsubz); break;
            case 0x0F: INSTR_EE(vmsubw); break;
            case 0x10: INSTR_EE(vmaxx); break;
            case 0x11: INSTR_EE(vmaxy); break;
            case 0x12: INSTR_EE(vmaxz); break;
            case 0x13: INSTR_EE(vmaxw); break;
            case 0x14: INSTR_EE(vminix); break;
            case 0x15: INSTR_EE(vminiy); break;
            case 0x16: INSTR_EE(vminiz); break;
            case 0x17: INSTR_EE(vminiw); break;
            case 0x18: INSTR_EE(vmulx); break;
            case 0x19: INSTR_EE(vmuly); break;
            case 0x1A: INSTR_EE(vmulz); break;
            case 0x1B: INSTR_EE(vmulw); break;
            case 0x1C: INSTR_EE(vmulq); break;
            case 0x1D: INSTR_EE(vmaxi); break;
            case 0x1E: INSTR_EE(vmuli); break;
            case 0x1F: INSTR_EE(vminii); break;
            case 0x20: INSTR_EE(vaddq); break;
            case 0x21: INSTR_EE(vmaddq); break;
            case 0x22: INSTR_EE(vaddi); break;
            case 0x23: INSTR_EE(vmaddi); break;
            case 0x24: INSTR_EE(vsubq); break;
            case 0x25: INSTR_EE(vmsubq); break;
            case 0x26: INSTR_EE(vsubi); break;
            case 0x27: INSTR_EE(vmsubi); break;
            case 0x28: INSTR_EE(vadd); break;
            case 0x29: INSTR_EE(vmadd); break;
            case 0x2A: INSTR_EE(vmul); break;
            case 0x2B: INSTR_EE(vmax); break;
            case 0x2C: INSTR_EE(vsub); break;
            case 0x2D: INSTR_EE(vmsub); break;
            case 0x2E: INSTR_EE(vopmsub); break;
            case 0x2F: INSTR_EE(vmini); break;
            case 0x30: INSTR_EE(viadd); break;
            case 0x31: INSTR_EE(visub); break;
            case 0x32: INSTR_EE(viaddi); break;
            case 0x34: INSTR_EE(viand); break;
            case 0x35: INSTR_EE(vior); break;
            case 0x38: INSTR_EE(vcallms); break;
            case 0x39: INSTR_EE(vcallmsr); break;
            case 0x3C:
            case 0x3D:
            case 0x3E:
            case 0x3F: { // Special2
                if ((instr & 0x3C) == 0x3C) {
                    switch (instr & 3 | (instr & 0xEC0) >> 4) {
                    case 0x00: INSTR_EE(vaddax); break;
                    case 0x01: INSTR_EE(vadday); break;
                    case 0x02: INSTR_EE(vaddaz); break;
                    case 0x03: INSTR_EE(vaddaw); break;
                    case 0x04: INSTR_EE(vsubax); break;
                    case 0x05: INSTR_EE(vsubay); break;
                    case 0x06: INSTR_EE(vsubaz); break;
                    case 0x07: INSTR_EE(vsubaw); break;
                    case 0x08: INSTR_EE(vmaddax); break;
                    case 0x09: INSTR_EE(vmadday); break;
                    case 0x0A: INSTR_EE(vmaddaz); break;
                    case 0x0B: INSTR_EE(vmaddaw); break;
                    case 0x0C: INSTR_EE(vmsubax); break;
                    case 0x0D: INSTR_EE(vmsubay); break;
                    case 0x0E: INSTR_EE(vmsubaz); break;
                    case 0x0F: INSTR_EE(vmsubaw); break;
                    case 0x10: INSTR_EE(vitof0); break;
                    case 0x11: INSTR_EE(vitof4); break;
                    case 0x12: INSTR_EE(vitof12); break;
                    case 0x13: INSTR_EE(vitof15); break;
                    case 0x14: INSTR_EE(vftoi0); break;
                    case 0x15: INSTR_EE(vftoi4); break;
                    case 0x16: INSTR_EE(vftoi12); break;
                    case 0x17: INSTR_EE(vftoi15); break;
                    case 0x18: INSTR_EE(vmulax); break;
                    case 0x19: INSTR_EE(vmulay); break;
                    case 0x1A: INSTR_EE(vmulaz); break;
                    case 0x1B: INSTR_EE(vmulaw); break;
                    case 0x1C: INSTR_EE(vmulaq); break;
                    case 0x1D: INSTR_EE(vabs); break;
                    case 0x1E: INSTR_EE(vmulai); break;
                    case 0x1F: INSTR_EE(vclipw); break;
                    case 0x20: INSTR_EE(vaddaq); break;
                    case 0x21: INSTR_EE(vmaddaq); break;
                    case 0x22: INSTR_EE(vaddai); break;
                    case 0x23: INSTR_EE(vmaddai); break;
                    case 0x24: INSTR_EE(vsubaq); break;
                    case 0x25: INSTR_EE(vmsubaq); break;
                    case 0x26: INSTR_EE(vsubai); break;
                    case 0x27: INSTR_EE(vmsubai); break;
                    case 0x28: INSTR_EE(vadda); break;
                    case 0x29: INSTR_EE(vmadda); break;
                    case 0x2A: INSTR_EE(vmula); break;
                    case 0x2C: INSTR_EE(vsuba); break;
                    case 0x2D: INSTR_EE(vmsuba); break;
                    case 0x2E: INSTR_EE(vopmula); break;
                    case 0x2F: INSTR_EE(vnop); break;
                    case 0x30: INSTR_EE(vmove); break;
                    case 0x31: INSTR_EE(vmr32); break;
                    case 0x34: INSTR_EE(vlqi); break;
                    case 0x35: INSTR_EE(vsqi); break;
                    case 0x36: INSTR_EE(vlqd); break;
                    case 0x37: INSTR_EE(vsqd); break;
                    case 0x38: INSTR_EE(vdiv); break;
                    case 0x39: INSTR_EE(vsqrt); break;
                    case 0x3A: INSTR_EE(vrsqrt); break;
                    case 0x3B: INSTR_EE(vwaitq); break;
                    case 0x3C: INSTR_EE(vmtir); break;
                    case 0x3D: INSTR_EE(vmfir); break;
                    case 0x3E: INSTR_EE(vilwr); break;
                    case 0x3F: INSTR_EE(viswr); break;
                    case 0x40: INSTR_EE(vrnext); break;
                    case 0x41: INSTR_EE(vrget); break;
                    case 0x42: INSTR_EE(vrinit); break;
                    case 0x43: INSTR_EE(vrxor); break;

                    default: reserved_instruction<cpu, make_string>(instr);
                    }
                } else {
                    reserved_instruction<cpu, make_string>(instr);
                }
            } break;

            default: reserved_instruction<cpu, make_string>(instr);
            }
        } else {
            switch (fmt) {
            case 1: INSTR_EE(qmfc2); break;
            case 2: INSTR_EE(cfc2); break;
            case 5: INSTR_EE(qmtc2); break;
            case 6: INSTR_EE(ctc2); break;
            case 8: { // BC2
                switch (instr >> 16 & 31) {
                case 0: INSTR_EE(bc2f); break;
                case 1: INSTR_EE(bc2t); break;
                case 2: INSTR_EE(bc2fl); break;
                case 3: INSTR_EE(bc2tl); break;
                default: reserved_instruction<cpu, make_string>(instr);
                }
            } break;
            default: reserved_instruction<cpu, make_string>(instr);
            }
        }

    } else {
        // TODO
    }
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void cop3(u32 instr)
{
    reserved_instruction<cpu, make_string>(instr);
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void disassemble(u32 instr)
{
    switch (instr >> 26 & 63) {
    case 0x00: special<cpu, cpu_impl, make_string>(instr); break;
    case 0x01: regimm<cpu, cpu_impl, make_string>(instr); break;
    case 0x02: INSTR_COMMON(j, IMM26); break;
    case 0x03: INSTR_COMMON(jal, IMM26); break;
    case 0x04: INSTR_COMMON(beq, RS, RT, IMM16); break;
    case 0x05: INSTR_COMMON(bne, RS, RT, IMM16); break;
    case 0x06: INSTR_COMMON(blez, RS, IMM16); break;
    case 0x07: INSTR_COMMON(bgtz, RS, IMM16); break;
    case 0x08: INSTR_COMMON(addi, RS, RT, IMM16); break;
    case 0x09: INSTR_COMMON(addiu, RS, RT, IMM16); break;
    case 0x0A: INSTR_COMMON(slti, RS, RT, IMM16); break;
    case 0x0B: INSTR_COMMON(sltiu, RS, RT, IMM16); break;
    case 0x0C: INSTR_COMMON(andi, RS, RT, IMM16); break;
    case 0x0D: INSTR_COMMON(ori, RS, RT, IMM16); break;
    case 0x0E: INSTR_COMMON(xori, RS, RT, IMM16); break;
    case 0x0F: INSTR_COMMON(lui, RT, IMM16); break;
    case 0x10: cop0<cpu, cpu_impl, make_string>(instr); break;
    case 0x11: cop1<cpu, cpu_impl, make_string>(instr); break;
    case 0x12: cop2<cpu, cpu_impl, make_string>(instr); break;
    case 0x13: cop3<cpu, cpu_impl, make_string>(instr); break;
    case 0x14: INSTR_EE(beql, RS, RT, IMM16); break;
    case 0x15: INSTR_EE(bnel, RS, RT, IMM16); break;
    case 0x16: INSTR_EE(blezl, RS, IMM16); break;
    case 0x17: INSTR_EE(bgtzl, RS, IMM16); break;
    case 0x18: INSTR_EE(daddi, RS, RT, IMM16); break;
    case 0x19: INSTR_EE(daddiu, RS, RT, IMM16); break;
    case 0x1A: INSTR_EE(ldl, RS, RT, IMM16); break;
    case 0x1B: INSTR_EE(ldr, RS, RT, IMM16); break;
    case 0x1C: mmi<cpu, cpu_impl, make_string>(instr); break;
    case 0x1E: INSTR_EE(lq, RS, RT, IMM16); break;
    case 0x1F: INSTR_EE(sq, RS, RT, IMM16); break;
    case 0x20: INSTR_COMMON(lb, RS, RT, IMM16); break;
    case 0x21: INSTR_COMMON(lh, RS, RT, IMM16); break;
    case 0x22: INSTR_COMMON(lwl, RS, RT, IMM16); break;
    case 0x23: INSTR_COMMON(lw, RS, RT, IMM16); break;
    case 0x24: INSTR_COMMON(lbu, RS, RT, IMM16); break;
    case 0x25: INSTR_COMMON(lhu, RS, RT, IMM16); break;
    case 0x26: INSTR_COMMON(lwr, RS, RT, IMM16); break;
    case 0x27: INSTR_COMMON(lwu, RS, RT, IMM16); break;
    case 0x28: INSTR_COMMON(sb, RS, RT, IMM16); break;
    case 0x29: INSTR_COMMON(sh, RS, RT, IMM16); break;
    case 0x2A: INSTR_COMMON(swl, RS, RT, IMM16); break;
    case 0x2B: INSTR_COMMON(sw, RS, RT, IMM16); break;
    case 0x2C: INSTR_EE(sdl, RS, RT, IMM16); break;
    case 0x2D: INSTR_EE(sdr, RS, RT, IMM16); break;
    case 0x2E: INSTR_COMMON(swr, RS, RT, IMM16); break;
    case 0x2F: INSTR_EE(cache); break;
    case 0x31: INSTR_EE(lwc1, FT, BASE, IMM16); break;
    case 0x33: INSTR_EE(pref); break;
    case 0x36: INSTR_EE(lqc2); break;
    case 0x37: INSTR_EE(ld, RS, RT, IMM16); break;
    case 0x39: INSTR_EE(swc1, FT, BASE, IMM16); break;
    case 0x3E: INSTR_EE(sqc2); break;
    case 0x3F: INSTR_EE(sd, RS, RT, IMM16); break;
    default: reserved_instruction<cpu, make_string>(instr);
    }
}

template<CpuImpl cpu_impl> void disassemble_ee(u32 instr)
{
    disassemble<Cpu::EE, cpu_impl, false>(instr);
}

template<CpuImpl cpu_impl> void disassemble_iop(u32 instr)
{
    disassemble<Cpu::IOP, cpu_impl, false>(instr);
}

std::string disassemble_str(u32 instr)
{
    // disassemble<Cpu::EE, cpu_impl, true>(instr);
    return disassemble_result;
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void mmi(u32 instr)
{
    if constexpr (cpu == Cpu::IOP) {
        reserved_instruction<Cpu::IOP, make_string>(instr);
        return;
    }

    auto mmi0 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        case 0x00: INSTR_EE(paddw, RS, RT, RD); break;
        case 0x01: INSTR_EE(psubw, RS, RT, RD); break;
        case 0x02: INSTR_EE(pcgtw, RS, RT, RD); break;
        case 0x03: INSTR_EE(pmaxw, RS, RT, RD); break;
        case 0x04: INSTR_EE(paddh, RS, RT, RD); break;
        case 0x05: INSTR_EE(psubh, RS, RT, RD); break;
        case 0x06: INSTR_EE(pcgth, RS, RT, RD); break;
        case 0x07: INSTR_EE(pmaxh, RS, RT, RD); break;
        case 0x08: INSTR_EE(paddb, RS, RT, RD); break;
        case 0x09: INSTR_EE(psubb, RS, RT, RD); break;
        case 0x0A: INSTR_EE(pcgtb, RS, RT, RD); break;
        case 0x10: INSTR_EE(paddsw, RS, RT, RD); break;
        case 0x11: INSTR_EE(psubsw, RS, RT, RD); break;
        case 0x12: INSTR_EE(pextlw, RS, RT, RD); break;
        case 0x13: INSTR_EE(ppacw, RS, RT, RD); break;
        case 0x14: INSTR_EE(paddsh, RS, RT, RD); break;
        case 0x15: INSTR_EE(psubsh, RS, RT, RD); break;
        case 0x16: INSTR_EE(pextlh, RS, RT, RD); break;
        case 0x17: INSTR_EE(ppach, RS, RT, RD); break;
        case 0x18: INSTR_EE(paddsb, RS, RT, RD); break;
        case 0x19: INSTR_EE(psubsb, RS, RT, RD); break;
        case 0x1A: INSTR_EE(pextlb, RS, RT, RD); break;
        case 0x1B: INSTR_EE(ppacb, RS, RT, RD); break;
        case 0x1E: INSTR_EE(pext5, RT, RD); break;
        case 0x1F: INSTR_EE(ppac5, RT, RD); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    };

    auto mmi1 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        case 0x01: INSTR_EE(pabsw, RT, RD); break;
        case 0x02: INSTR_EE(pceqw, RS, RT, RD); break;
        case 0x03: INSTR_EE(pminw, RS, RT, RD); break;
        case 0x04: INSTR_EE(padsbh, RS, RT, RD); break;
        case 0x05: INSTR_EE(pabsh, RT, RD); break;
        case 0x06: INSTR_EE(pceqh, RS, RT, RD); break;
        case 0x07: INSTR_EE(pminh, RS, RT, RD); break;
        case 0x0A: INSTR_EE(pceqb, RS, RT, RD); break;
        case 0x10: INSTR_EE(padduw, RS, RT, RD); break;
        case 0x11: INSTR_EE(psubuw, RS, RT, RD); break;
        case 0x12: INSTR_EE(pextuw, RS, RT, RD); break;
        case 0x14: INSTR_EE(padduh, RS, RT, RD); break;
        case 0x15: INSTR_EE(psubuh, RS, RT, RD); break;
        case 0x16: INSTR_EE(pextuh, RS, RT, RD); break;
        case 0x18: INSTR_EE(paddub, RS, RT, RD); break;
        case 0x19: INSTR_EE(psubub, RS, RT, RD); break;
        case 0x1A: INSTR_EE(pextub, RS, RT, RD); break;
        case 0x1B: INSTR_EE(qfsrv, RS, RT, RD); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    };

    auto mmi2 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        case 0x00: INSTR_EE(pmaddw, RS, RT, RD); break;
        case 0x02: INSTR_EE(psllvw, RS, RT, RD); break;
        case 0x03: INSTR_EE(psrlvw, RS, RT, RD); break;
        case 0x04: INSTR_EE(pmsubw, RS, RT, RD); break;
        case 0x08: INSTR_EE(pmfhi, RD); break;
        case 0x09: INSTR_EE(pmflo, RD); break;
        case 0x0A: INSTR_EE(pinth, RS, RT, RD); break;
        case 0x0C: INSTR_EE(pmultw, RS, RT, RD); break;
        case 0x0D: INSTR_EE(pdivw, RS, RT); break;
        case 0x0E: INSTR_EE(pcpyld, RS, RT, RD); break;
        case 0x10: INSTR_EE(pmaddh, RS, RT, RD); break;
        case 0x11: INSTR_EE(phmadh, RS, RT, RD); break;
        case 0x12: INSTR_EE(pand, RS, RT, RD); break;
        case 0x13: INSTR_EE(pxor, RS, RT, RD); break;
        case 0x14: INSTR_EE(pmsubh, RS, RT, RD); break;
        case 0x15: INSTR_EE(phmsbh, RS, RT, RD); break;
        case 0x1A: INSTR_EE(pexeh, RT, RD); break;
        case 0x1B: INSTR_EE(prevh, RT, RD); break;
        case 0x1C: INSTR_EE(pmulth, RS, RT, RD); break;
        case 0x1D: INSTR_EE(pdivbw, RS, RT); break;
        case 0x1E: INSTR_EE(pexew, RT, RD); break;
        case 0x1F: INSTR_EE(prot3w, RT, RD); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    };

    auto mmi3 = [](u32 instr) {
        switch (instr >> 6 & 31) {
        case 0x00: INSTR_EE(pmadduw, RS, RT, RD); break;
        case 0x03: INSTR_EE(psravw, RS, RT, RD); break;
        case 0x08: INSTR_EE(pmthi, RS); break;
        case 0x09: INSTR_EE(pmtlo, RS); break;
        case 0x0A: INSTR_EE(pinteh, RS, RT, RD); break;
        case 0x0C: INSTR_EE(pmultuw, RS, RT, RD); break;
        case 0x0D: INSTR_EE(pdivuw, RS, RT); break;
        case 0x0E: INSTR_EE(pcpyud, RS, RT, RD); break;
        case 0x12: INSTR_EE(por, RS, RT, RD); break;
        case 0x13: INSTR_EE(pnor, RS, RT, RD); break;
        case 0x1A: INSTR_EE(pexch, RT, RD); break;
        case 0x1B: INSTR_EE(pcpyh, RT, RD); break;
        case 0x1E: INSTR_EE(pexcw, RT, RD); break;
        default: reserved_instruction<cpu, make_string>(instr);
        }
    };

    switch (instr & 63) {
    case 0x00: INSTR_EE(madd, RS, RT, RD); break;
    case 0x01: INSTR_EE(maddu, RS, RT, RD); break;
    case 0x04: INSTR_EE(plzcw, RS, RD); break;
    case 0x08: mmi0(instr); break;
    case 0x09: mmi2(instr); break;
    case 0x10: INSTR_EE(mfhi1, RD); break;
    case 0x11: INSTR_EE(mthi1, RS); break;
    case 0x12: INSTR_EE(mflo1, RD); break;
    case 0x13: INSTR_EE(mtlo1, RS); break;
    case 0x18: INSTR_EE(mult1, RS, RT); break;
    case 0x19: INSTR_EE(multu1, RS, RT); break;
    case 0x1A: INSTR_EE(div1, RS, RT); break;
    case 0x1B: INSTR_EE(divu1, RS, RT); break;
    case 0x20: INSTR_EE(madd1, RS, RT, RD); break;
    case 0x21: INSTR_EE(maddu1, RS, RT, RD); break;
    case 0x28: mmi1(instr); break;
    case 0x29: mmi3(instr); break;
    case 0x30: INSTR_EE(pmfhl, RD, FMT); break;
    case 0x31: INSTR_EE(pmthl, RS, FMT); break;
    case 0x34: INSTR_EE(psllh, RT, RD, SA); break;
    case 0x36: INSTR_EE(psrlh, RT, RD, SA); break;
    case 0x37: INSTR_EE(psrah, RT, RD, SA); break;
    case 0x3C: INSTR_EE(psllw, RT, RD, SA); break;
    case 0x3E: INSTR_EE(psrlw, RT, RD, SA); break;
    case 0x3F: INSTR_EE(psraw, RT, RD, SA); break;
    default: reserved_instruction<cpu, make_string>(instr);
    }
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void regimm(u32 instr)
{
    switch (instr >> 16 & 31) {
    case 0x00: INSTR_COMMON(bltz, RS, IMM16); break;
    case 0x01: INSTR_COMMON(bgez, RS, IMM16); break;
    case 0x02: INSTR_EE(bltzl, RS, IMM16); break;
    case 0x03: INSTR_EE(bgezl, RS, IMM16); break;
    case 0x08: INSTR_EE(tgei, RS, IMM16); break;
    case 0x09: INSTR_EE(tgeiu, RS, IMM16); break;
    case 0x0A: INSTR_EE(tlti, RS, IMM16); break;
    case 0x0B: INSTR_EE(tltiu, RS, IMM16); break;
    case 0x0C: INSTR_EE(teqi, RS, IMM16); break;
    case 0x0E: INSTR_EE(tnei, RS, IMM16); break;
    case 0x10: INSTR_COMMON(bltzal, RS, IMM16); break;
    case 0x11: INSTR_COMMON(bgezal, RS, IMM16); break;
    case 0x12: INSTR_EE(bltzall, RS, IMM16); break;
    case 0x13: INSTR_EE(bgezall, RS, IMM16); break;
    case 0x18: INSTR_EE(mtsab, RS, IMM16); break;
    case 0x19: INSTR_EE(mtsah, RS, IMM16); break;
    default: reserved_instruction<cpu, make_string>(instr);
    }
}

template<Cpu cpu, bool make_string> void reserved_instruction(auto instr)
{
    if constexpr (make_string) {

    } else if constexpr (cpu == Cpu::EE) {
        ee::reserved_instruction_exception();
    } else {
        iop::reserved_instruction_exception();
    }
    // TODO: log
}

template<Cpu cpu, CpuImpl cpu_impl, bool make_string> void special(u32 instr)
{
    switch (instr & 63) {
    case 0x00: INSTR_COMMON(sll, RT, RD, SA); break;
    case 0x02: INSTR_COMMON(srl, RT, RD, SA); break;
    case 0x03: INSTR_COMMON(sra, RT, RD, SA); break;
    case 0x04: INSTR_COMMON(sllv, RS, RT, RD); break;
    case 0x06: INSTR_COMMON(srlv, RS, RT, RD); break;
    case 0x07: INSTR_COMMON(srav, RS, RT, RD); break;
    case 0x08: INSTR_COMMON(jr, RS); break;
    case 0x09: INSTR_COMMON(jalr, RS, RD); break;
    case 0x0A: INSTR_EE(movz, RS, RT, RD); break;
    case 0x0B: INSTR_EE(movn, RS, RT, RD); break;
    case 0x0C: INSTR_COMMON(syscall); break;
    case 0x0D: INSTR_COMMON(break_); break;
    case 0x0F: INSTR_EE(sync); break;
    case 0x10: INSTR_COMMON(mfhi, RD); break;
    case 0x11: INSTR_COMMON(mthi, RS); break;
    case 0x12: INSTR_COMMON(mflo, RD); break;
    case 0x13: INSTR_COMMON(mtlo, RS); break;
    case 0x14: INSTR_EE(dsllv, RS, RT, RD); break;
    case 0x16: INSTR_EE(dsrlv, RS, RT, RD); break;
    case 0x17: INSTR_EE(dsrav, RS, RT, RD); break;
    case 0x18: INSTR_COMMON(mult, RS, RT); break;
    case 0x19: INSTR_COMMON(multu, RS, RT); break;
    case 0x1A: INSTR_COMMON(div, RS, RT); break;
    case 0x1B: INSTR_COMMON(divu, RS, RT); break;
    case 0x20: INSTR_COMMON(add, RS, RT, RD); break;
    case 0x21: INSTR_COMMON(addu, RS, RT, RD); break;
    case 0x22: INSTR_COMMON(sub, RS, RT, RD); break;
    case 0x23: INSTR_COMMON(subu, RS, RT, RD); break;
    case 0x24: INSTR_COMMON(and_, RS, RT, RD); break;
    case 0x25: INSTR_COMMON(or_, RS, RT, RD); break;
    case 0x26: INSTR_COMMON(xor_, RS, RT, RD); break;
    case 0x27: INSTR_COMMON(nor, RS, RT, RD); break;
    case 0x28: INSTR_EE(mfsa, RD); break;
    case 0x29: INSTR_EE(mtsa, RS); break;
    case 0x2A: INSTR_COMMON(slt, RS, RT, RD); break;
    case 0x2B: INSTR_COMMON(sltu, RS, RT, RD); break;
    case 0x2C: INSTR_EE(dadd, RS, RT, RD); break;
    case 0x2D: INSTR_EE(daddu, RS, RT, RD); break;
    case 0x2E: INSTR_EE(dsub, RS, RT, RD); break;
    case 0x2F: INSTR_EE(dsubu, RS, RT, RD); break;
    case 0x30: INSTR_EE(tge, RS, RT); break;
    case 0x31: INSTR_EE(tgeu, RS, RT); break;
    case 0x32: INSTR_EE(tlt, RS, RT); break;
    case 0x33: INSTR_EE(tltu, RS, RT); break;
    case 0x34: INSTR_EE(teq, RS, RT); break;
    case 0x36: INSTR_EE(tne, RS, RT); break;
    case 0x38: INSTR_EE(dsll, RT, RD, SA); break;
    case 0x3A: INSTR_EE(dsrl, RT, RD, SA); break;
    case 0x3B: INSTR_EE(dsra, RT, RD, SA); break;
    case 0x3C: INSTR_EE(dsll32, RT, RD, SA); break;
    case 0x3E: INSTR_EE(dsrl32, RT, RD, SA); break;
    case 0x3F: INSTR_EE(dsra32, RT, RD, SA); break;
    default: reserved_instruction<cpu, make_string>(instr);
    }
}

template void disassemble_ee<CpuImpl::Interpreter>(u32);
// template void disassemble_ee<CpuImpl::Recompiler>(u32);
template void disassemble_iop<CpuImpl::Interpreter>(u32);
// template void disassemble_iop<CpuImpl::Recompiler>(u32);

} // namespace mips
