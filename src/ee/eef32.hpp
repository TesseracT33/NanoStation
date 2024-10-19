#pragma once

#include "numtypes.hpp"

#include <bit>

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
