#pragma once

#include "eef32.hpp"
#include "mips/mips.hpp"
#include "types.hpp"

#include <array>
#include <bit>
#include <cstring>

namespace ee::vu {

using Vf = std::array<EeF32, 4>; // x, y, z, w

struct Gpr {
    Vf get(u32 idx) const { return gpr[idx]; }
    void set(u32 idx, Vf data)
    {
        gpr[idx] = data;
        std::memcpy(&gpr[0], &zero, 16); // don't assign directly from 'zero' to avoid ieee754 conversion from happening
    }
    void set(u32 idx, u32 lane, auto data)
        requires(sizeof(data) == 4)
    {
        gpr[idx][lane] = std::bit_cast<f32>(data);
        std::memcpy(&gpr[0], &zero, 16);
    }

    Vf operator[](u32 idx) const { return gpr[idx]; } // return by value so that writes have to be made through 'set'

private:
    std::array<Vf, 32> gpr{};
    static constexpr Vf zero{ 0.f, 0.f, 0.f, 1.f };
} inline vf;

inline std::array<u32, 16> vi;
inline EeF32 I, P, Q, R;
alignas(16) inline Vf acc;

struct {
    bool zero[4]; // x, y, z, w
    bool sign[4];
    bool underflow[4];
    bool overflow[4];
    u16 to_u16() const
    {
        u16 ret{};
        for (int i = 0; i < 4; ++i) {
            ret |= zero[3 - i] << i;
            ret |= sign[3 - i] << (i + 4);
            ret |= underflow[3 - i] << (i + 8);
            ret |= overflow[3 - i] << (i + 12);
        }
        return ret;
    }
    void update(int dst, EeF32 result)
    {
        s32 result_s32 = std::bit_cast<s32>(result);
        s32 exponent = result_s32 & 0x7F80'0000;
        zero[dst] = result == 0.f;
        sign[dst] = result_s32 < 0;
        underflow[dst] = exponent == 0;
        overflow[dst] = exponent == 0x7F80'0000;
    }
} inline mac;

struct {
    bool zero, sign, underflow, overflow, invalid, div_zero, sticky_zero, sticky_sign, sticky_underflow,
      sticky_overflow, sticky_invalid, sticky_div_zero;
    void signal_invalid()
    {
        invalid = 1;
        sticky_invalid |= 1;
    }
    u16 to_u16() const
    {
        return zero | sign << 1 | underflow << 2 | overflow << 3 | invalid << 4 | div_zero << 5 | sticky_zero << 6
             | sticky_sign << 7 | sticky_underflow << 8 | sticky_overflow << 9 | sticky_invalid << 10
             | sticky_div_zero << 11;
    }
} inline status;

template<mips::CpuImpl> void vabs(u32 instr);
template<mips::CpuImpl> void vadd(u32 instr);
template<mips::CpuImpl> void vadda(u32 instr);
template<mips::CpuImpl> void vaddai(u32 instr);
template<mips::CpuImpl> void vaddaq(u32 instr);
template<mips::CpuImpl> void vaddabc(u32 instr);
template<mips::CpuImpl> void vaddi(u32 instr);
template<mips::CpuImpl> void vaddq(u32 instr);
template<mips::CpuImpl> void vaddbc(u32 instr);
template<mips::CpuImpl> void vcallms(u32 instr);
template<mips::CpuImpl> void vcallmsr(u32 instr); // called callmsr?
template<mips::CpuImpl> void vclipw(u32 instr);
template<mips::CpuImpl> void vdiv(u32 instr);
template<mips::CpuImpl> void vftoi0(u32 instr);
template<mips::CpuImpl> void vftoi4(u32 instr);
template<mips::CpuImpl> void vftoi12(u32 instr);
template<mips::CpuImpl> void vftoi15(u32 instr);
template<mips::CpuImpl> void viadd(u32 instr);
template<mips::CpuImpl> void viaddi(u32 instr);
template<mips::CpuImpl> void viand(u32 instr);
template<mips::CpuImpl> void vilwr(u32 instr);
template<mips::CpuImpl> void vior(u32 instr);
template<mips::CpuImpl> void visub(u32 instr);
template<mips::CpuImpl> void viswr(u32 instr);
template<mips::CpuImpl> void vitof0(u32 instr);
template<mips::CpuImpl> void vitof4(u32 instr);
template<mips::CpuImpl> void vitof12(u32 instr);
template<mips::CpuImpl> void vitof15(u32 instr);
template<mips::CpuImpl> void vlqd(u32 instr);
template<mips::CpuImpl> void vlqi(u32 instr);
template<mips::CpuImpl> void vmadd(u32 instr);
template<mips::CpuImpl> void vmadda(u32 instr);
template<mips::CpuImpl> void vmaddai(u32 instr);
template<mips::CpuImpl> void vmaddaq(u32 instr);
template<mips::CpuImpl> void vmaddabc(u32 instr);
template<mips::CpuImpl> void vmaddi(u32 instr);
template<mips::CpuImpl> void vmaddq(u32 instr);
template<mips::CpuImpl> void vmaddbc(u32 instr);
template<mips::CpuImpl> void vmax(u32 instr);
template<mips::CpuImpl> void vmaxi(u32 instr);
template<mips::CpuImpl> void vmaxbc(u32 instr);
template<mips::CpuImpl> void vmfir(u32 instr);
template<mips::CpuImpl> void vmini(u32 instr);
template<mips::CpuImpl> void vminii(u32 instr);
template<mips::CpuImpl> void vminibc(u32 instr);
template<mips::CpuImpl> void vmove(u32 instr);
template<mips::CpuImpl> void vmr32(u32 instr);
template<mips::CpuImpl> void vmsub(u32 instr);
template<mips::CpuImpl> void vmsubai(u32 instr);
template<mips::CpuImpl> void vmsubaq(u32 instr);
template<mips::CpuImpl> void vmsubabc(u32 instr);
template<mips::CpuImpl> void vmsuba(u32 instr);
template<mips::CpuImpl> void vmsubi(u32 instr);
template<mips::CpuImpl> void vmsubq(u32 instr);
template<mips::CpuImpl> void vmsubbc(u32 instr);
template<mips::CpuImpl> void vmtir(u32 instr);
template<mips::CpuImpl> void vmul(u32 instr);
template<mips::CpuImpl> void vmula(u32 instr);
template<mips::CpuImpl> void vmulai(u32 instr);
template<mips::CpuImpl> void vmulaq(u32 instr);
template<mips::CpuImpl> void vmulabc(u32 instr);
template<mips::CpuImpl> void vmuli(u32 instr);
template<mips::CpuImpl> void vmulq(u32 instr);
template<mips::CpuImpl> void vmulbc(u32 instr);
template<mips::CpuImpl> void vnop(u32 instr);
template<mips::CpuImpl> void vopmsub(u32 instr);
template<mips::CpuImpl> void vopmula(u32 instr);
template<mips::CpuImpl> void vrget(u32 instr);
template<mips::CpuImpl> void vrinit(u32 instr);
template<mips::CpuImpl> void vrnext(u32 instr);
template<mips::CpuImpl> void vrsqrt(u32 instr);
template<mips::CpuImpl> void vrxor(u32 instr);
template<mips::CpuImpl> void vsqd(u32 instr);
template<mips::CpuImpl> void vsqi(u32 instr);
template<mips::CpuImpl> void vsqrt(u32 instr);
template<mips::CpuImpl> void vsub(u32 instr);
template<mips::CpuImpl> void vsuba(u32 instr);
template<mips::CpuImpl> void vsubai(u32 instr);
template<mips::CpuImpl> void vsubaq(u32 instr);
template<mips::CpuImpl> void vsubabc(u32 instr);
template<mips::CpuImpl> void vsubi(u32 instr);
template<mips::CpuImpl> void vsubq(u32 instr);
template<mips::CpuImpl> void vsubbc(u32 instr);
template<mips::CpuImpl> void vwaitq(u32 instr);

} // namespace ee::vu
