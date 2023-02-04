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
    template<uint lane = 0>::s8 s8() const { return get<::s8, lane>(); }
    template<uint lane = 0>::u8 u8() const { return get<::u8, lane>(); }
    template<uint lane = 0>::s16 s16() const { return get<::s16, lane>(); }
    template<uint lane = 0>::u16 u16() const { return get<::u16, lane>(); }
    template<uint lane = 0>::s32 s32() const { return get<::s32, lane>(); }
    template<uint lane = 0>::u32 u32() const { return get<::u32, lane>(); }
    template<uint lane = 0>::s64 s64() const { return get<::s64, lane>(); }
    template<uint lane = 0>::u64 u64() const { return get<::u64, lane>(); }
    ::m128i m128i() const { return std::bit_cast<::m128i>(*this); }
    ::u64 get_lower_dword() { return lo; }
    ::u64 get_upper_dword() { return hi; }
    void set(std::integral auto val) { lo = val; }
    void set_lower_dword(std::integral auto val) { set(val); }
    void set_upper_dword(std::integral auto val) { hi = val; }
    void set(::m128i val) { *this = std::bit_cast<Reg128>(val); }
    void set(u64x2 val) { *this = std::bit_cast<Reg128>(val); }

    template<std::integral Int> Int get() const
    {
        Int ret;
        std::memcpy(&ret, reinterpret_cast<::u8 const*>(this), sizeof(Int));
        return ret;
    }

    template<std::integral Int, uint lane> Int get() const
    {
        static_assert(lane < sizeof(*this) / sizeof(Int), "Accessing out-of-bounds lane.");
        Int ret;
        std::memcpy(&ret, reinterpret_cast<::u8 const*>(this) + lane * sizeof(Int), sizeof(Int));
        return ret;
    }

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

    operator ::m128i() const { return m128i(); }

private:
    ::u64 lo, hi;
};
