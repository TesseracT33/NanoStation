#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

#include <array>
#include <bit>
#include <cstring>

namespace ee::vu {

struct EeF32 {
    constexpr EeF32() = default;
    constexpr EeF32(f32 f) { value = ieee754_to_ee(f); }
    constexpr EeF32& operator=(f32 f)
    {
        value = ieee754_to_ee(f);
        return *this;
    }
    constexpr operator f32() const { return value; }

private:
    static constexpr f32 ieee754_to_ee(f32 f)
    {
        // There are major differences between the PS2 floating-point format and standard IEEE 754, used in most modern
        // computers.
        // * NaNs and Infinities do not exist on the PS2. In IEEE 754, any number with the exponent field set to 255 is
        // a NaN/Infinity. On the PS2, it is treated as a normal value. This means the upper range of PS2 floats is
        // greater than on IEEE 754.
        // * Denormals, numbers with the exponent field set to 0, do not exist on the PS2. For every
        // arithmetic operation, a denormal is automatically truncated to zero. On the PS2, the rounding mode is always
        // forced to round-towards-zero.
        // * The PS2 uses a quirky version where the least-significant-bit may vary after a
        // round, and the details on this are not well understood.
        u32 result = std::bit_cast<u32>(f);
        u32 exp = result & 0x7F80'0000;
        if (exp == 0) {
            result &= 0x8000'0000; // return zero, preserve sign
        }
        if (exp == 0x7F80'0000) {
            result = (result & 0x8000'0000) | 0x7F7F'FFFF; // return IEEE-754 F32_MIN/F32_MAX, preserve sign
        }
        return std::bit_cast<f32>(result);
    }

    f32 value{};
};

using Vf = std::array<EeF32, 4>;

struct Gpr {
    Vf get(u32 idx) const { return gpr[idx]; }
    void set(u32 idx, Vf data)
    {
        gpr[idx] = data;
        std::memcpy(&gpr[0], &zero, 16); // don't assign directly from 'zero' to avoid ieee754 conversion from happening
    }
    void set(u32 idx, u32 lane, f32 data)
    {
        gpr[idx][lane] = data;
        std::memcpy(&gpr[0], &zero, 16);
    }

    Vf operator[](u32 idx) const { return gpr[idx]; } // return by value so that writes have to be made through 'set'

private:
    std::array<Vf, 32> gpr{};
    static constexpr Vf zero{ 0.f, 0.f, 0.f, 1.f };
} inline vf;

inline std::array<u16, 16> vi;
inline EeF32 p, q;
alignas(16) inline Vf acc;

struct {
    bool zero[4]; // w, z, y, x
    bool sign[4];
    bool underflow[4];
    bool overflow[4];
    u16 to_u16() const
    {
        u16 ret{};
        for (int i = 0; i < 4; ++i) {
            ret |= zero[i] << i;
            ret |= sign[i] << (i + 4);
            ret |= underflow[i] << (i + 8);
            ret |= overflow[i] << (i + 12);
        }
        return ret;
    }
} inline mac;

struct {
    bool zero, sign, underflow, overflow, invalid, div_zero, sticky_zero, sticky_sign, sticky_underflow,
      sticky_overflow, sticky_invalid, sticky_div_zero;
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
template<mips::CpuImpl> void vaddaw(u32 instr);
template<mips::CpuImpl> void vaddax(u32 instr);
template<mips::CpuImpl> void vadday(u32 instr);
template<mips::CpuImpl> void vaddaz(u32 instr);
template<mips::CpuImpl> void vaddi(u32 instr);
template<mips::CpuImpl> void vaddq(u32 instr);
template<mips::CpuImpl> void vaddw(u32 instr);
template<mips::CpuImpl> void vaddx(u32 instr);
template<mips::CpuImpl> void vaddy(u32 instr);
template<mips::CpuImpl> void vaddz(u32 instr);
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
template<mips::CpuImpl> void vmaddaw(u32 instr);
template<mips::CpuImpl> void vmaddax(u32 instr);
template<mips::CpuImpl> void vmadday(u32 instr);
template<mips::CpuImpl> void vmaddaz(u32 instr);
template<mips::CpuImpl> void vmaddi(u32 instr);
template<mips::CpuImpl> void vmaddq(u32 instr);
template<mips::CpuImpl> void vmaddw(u32 instr);
template<mips::CpuImpl> void vmaddx(u32 instr);
template<mips::CpuImpl> void vmaddy(u32 instr);
template<mips::CpuImpl> void vmaddz(u32 instr);
template<mips::CpuImpl> void vmax(u32 instr);
template<mips::CpuImpl> void vmaxi(u32 instr);
template<mips::CpuImpl> void vmaxw(u32 instr);
template<mips::CpuImpl> void vmaxx(u32 instr);
template<mips::CpuImpl> void vmaxy(u32 instr);
template<mips::CpuImpl> void vmaxz(u32 instr);
template<mips::CpuImpl> void vmfir(u32 instr);
template<mips::CpuImpl> void vmini(u32 instr);
template<mips::CpuImpl> void vminii(u32 instr);
template<mips::CpuImpl> void vminiw(u32 instr);
template<mips::CpuImpl> void vminix(u32 instr);
template<mips::CpuImpl> void vminiy(u32 instr);
template<mips::CpuImpl> void vminiz(u32 instr);
template<mips::CpuImpl> void vmove(u32 instr);
template<mips::CpuImpl> void vmr32(u32 instr);
template<mips::CpuImpl> void vmsub(u32 instr);
template<mips::CpuImpl> void vmsubai(u32 instr);
template<mips::CpuImpl> void vmsubaq(u32 instr);
template<mips::CpuImpl> void vmsubaw(u32 instr);
template<mips::CpuImpl> void vmsubax(u32 instr);
template<mips::CpuImpl> void vmsubay(u32 instr);
template<mips::CpuImpl> void vmsubaz(u32 instr);
template<mips::CpuImpl> void vmsuba(u32 instr);
template<mips::CpuImpl> void vmsubi(u32 instr);
template<mips::CpuImpl> void vmsubq(u32 instr);
template<mips::CpuImpl> void vmsubw(u32 instr);
template<mips::CpuImpl> void vmsubx(u32 instr);
template<mips::CpuImpl> void vmsuby(u32 instr);
template<mips::CpuImpl> void vmsubz(u32 instr);
template<mips::CpuImpl> void vmtir(u32 instr);
template<mips::CpuImpl> void vmul(u32 instr);
template<mips::CpuImpl> void vmula(u32 instr);
template<mips::CpuImpl> void vmulai(u32 instr);
template<mips::CpuImpl> void vmulaq(u32 instr);
template<mips::CpuImpl> void vmulaw(u32 instr);
template<mips::CpuImpl> void vmulax(u32 instr);
template<mips::CpuImpl> void vmulay(u32 instr);
template<mips::CpuImpl> void vmulaz(u32 instr);
template<mips::CpuImpl> void vmuli(u32 instr);
template<mips::CpuImpl> void vmulq(u32 instr);
template<mips::CpuImpl> void vmulw(u32 instr);
template<mips::CpuImpl> void vmulx(u32 instr);
template<mips::CpuImpl> void vmuly(u32 instr);
template<mips::CpuImpl> void vmulz(u32 instr);
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
template<mips::CpuImpl> void vsubaw(u32 instr);
template<mips::CpuImpl> void vsubax(u32 instr);
template<mips::CpuImpl> void vsubay(u32 instr);
template<mips::CpuImpl> void vsubaz(u32 instr);
template<mips::CpuImpl> void vsubi(u32 instr);
template<mips::CpuImpl> void vsubq(u32 instr);
template<mips::CpuImpl> void vsubw(u32 instr);
template<mips::CpuImpl> void vsubx(u32 instr);
template<mips::CpuImpl> void vsuby(u32 instr);
template<mips::CpuImpl> void vsubz(u32 instr);
template<mips::CpuImpl> void vwaitq(u32 instr);

} // namespace ee::vu
