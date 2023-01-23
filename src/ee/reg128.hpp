#pragma once

#include "host.hpp"
#include "types.hpp"

#include <bit>
#include <concepts>
#include <cstring>

#if X64
#include <immintrin.h>
using m128i = __m128i;
#endif

struct Reg128 {
    template<uint lane = 0>::s8 s8() const { return get<lane, ::s8>(); }
    template<uint lane = 0>::u8 u8() const { return get<lane, ::u8>(); }
    template<uint lane = 0>::s16 s16() const { return get<lane, ::s16>(); }
    template<uint lane = 0>::u16 u16() const { return get<lane, ::u16>(); }
    template<uint lane = 0>::s32 s32() const { return get<lane, ::s32>(); }
    template<uint lane = 0>::u32 u32() const { return get<lane, ::u32>(); }
    template<uint lane = 0>::s64 s64() const { return get<lane, ::s64>(); }
    template<uint lane = 0>::u64 u64() const { return get<lane, ::u64>(); }
    ::m128i m128i() const { return std::bit_cast<::m128i>(*this); }
    void set(std::integral auto val) { lo = val; }
    void set(::m128i val) { *this = std::bit_cast<Reg128>(val); }

    template<uint lane> void set(std::integral auto val)
    {
        static_assert(lane < sizeof(*this) / sizeof(val), "Accessing out-of-bounds lane.");
        std::memcpy(reinterpret_cast<::u8*>(this) + lane * sizeof(val), &val, sizeof(val));
    }

    Reg128& operator=(std::integral auto val) noexcept
    {
        set(val);
        return *this;
    }

    Reg128& operator=(::m128i val) noexcept
    {
        set(val);
        return *this;
    }

private:
    template<uint lane, std::integral Int> Int get() const
    {
        static_assert(lane < sizeof(*this) / sizeof(Int), "Accessing out-of-bounds lane.");
        Int ret;
        std::memcpy(&ret, reinterpret_cast<::u8 const*>(this) + lane * sizeof(Int), sizeof(Int));
        return ret;
    }

    ::u64 lo, hi;
};
