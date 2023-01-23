#pragma once

#include "host.hpp"

#include <bit>
#include <concepts>
#include <cstddef>
#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using s8 = int8_t;
using s16 = int16_t;
using s32 = int32_t;
using s64 = int64_t;

using uint = unsigned;

#if X64
#include <immintrin.h>
using m128i = __m128i;
#endif

struct Reg128 {
    ::s8 s8() const { return ::s8(lo); }
    ::u8 u8() const { return ::u8(lo); }
    ::s16 s16() const { return ::s16(lo); }
    ::u16 u16() const { return ::u16(lo); }
    ::s32 s32() const { return ::s32(lo); }
    ::u32 u32() const { return ::u32(lo); }
    ::s64 s64() const { return ::s64(lo); }
    ::u64 u64() const { return ::u64(lo); }
    ::m128i m128i() const { return std::bit_cast<::m128i>(*this); }
    void set(std::integral auto val) { lo = val; }
    void set(::m128i val) { *this = std::bit_cast<Reg128>(val); }

private:
    ::u64 lo, hi;
};

using std::size_t;
