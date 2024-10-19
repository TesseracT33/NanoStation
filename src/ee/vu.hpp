#pragma once

#include "eef32.hpp"
#include "mips/mips.hpp"
#include "numtypes.hpp"

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

void vabs(u32 instr);
void vadd(u32 instr);
void vadda(u32 instr);
void vaddai(u32 instr);
void vaddaq(u32 instr);
void vaddabc(u32 instr);
void vaddi(u32 instr);
void vaddq(u32 instr);
void vaddbc(u32 instr);
void vcallms(u32 instr);
void vcallmsr(u32 instr); // called callmsr?
void vclipw(u32 instr);
void vdiv(u32 instr);
void vftoi0(u32 instr);
void vftoi4(u32 instr);
void vftoi12(u32 instr);
void vftoi15(u32 instr);
void viadd(u32 instr);
void viaddi(u32 instr);
void viand(u32 instr);
void vilwr(u32 instr);
void vior(u32 instr);
void visub(u32 instr);
void viswr(u32 instr);
void vitof0(u32 instr);
void vitof4(u32 instr);
void vitof12(u32 instr);
void vitof15(u32 instr);
void vlqd(u32 instr);
void vlqi(u32 instr);
void vmadd(u32 instr);
void vmadda(u32 instr);
void vmaddai(u32 instr);
void vmaddaq(u32 instr);
void vmaddabc(u32 instr);
void vmaddi(u32 instr);
void vmaddq(u32 instr);
void vmaddbc(u32 instr);
void vmax(u32 instr);
void vmaxi(u32 instr);
void vmaxbc(u32 instr);
void vmfir(u32 instr);
void vmini(u32 instr);
void vminii(u32 instr);
void vminibc(u32 instr);
void vmove(u32 instr);
void vmr32(u32 instr);
void vmsub(u32 instr);
void vmsubai(u32 instr);
void vmsubaq(u32 instr);
void vmsubabc(u32 instr);
void vmsuba(u32 instr);
void vmsubi(u32 instr);
void vmsubq(u32 instr);
void vmsubbc(u32 instr);
void vmtir(u32 instr);
void vmul(u32 instr);
void vmula(u32 instr);
void vmulai(u32 instr);
void vmulaq(u32 instr);
void vmulabc(u32 instr);
void vmuli(u32 instr);
void vmulq(u32 instr);
void vmulbc(u32 instr);
void vnop(u32 instr);
void vopmsub(u32 instr);
void vopmula(u32 instr);
void vrget(u32 instr);
void vrinit(u32 instr);
void vrnext(u32 instr);
void vrsqrt(u32 instr);
void vrxor(u32 instr);
void vsqd(u32 instr);
void vsqi(u32 instr);
void vsqrt(u32 instr);
void vsub(u32 instr);
void vsuba(u32 instr);
void vsubai(u32 instr);
void vsubaq(u32 instr);
void vsubabc(u32 instr);
void vsubi(u32 instr);
void vsubq(u32 instr);
void vsubbc(u32 instr);
void vwaitq(u32 instr);

} // namespace ee::vu
