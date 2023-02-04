#pragma once

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

#ifdef __SIZEOF_INT128__
#define INT128_AVAILABLE 1
using u128 = __uint128_t;
using s128 = __int128_t;
#else
using u128 = struct {
    u64 lo, hi;
};
using s128 = struct {
    s64 lo, hi;
};
#endif

using uint = unsigned;

using std::size_t;

template<size_t> struct SizeToUInt {};

template<> struct SizeToUInt<1> {
    using type = u8;
};

template<> struct SizeToUInt<2> {
    using type = u16;
};

template<> struct SizeToUInt<4> {
    using type = u32;
};

template<> struct SizeToUInt<8> {
    using type = u64;
};

template<> struct SizeToUInt<16> {
    using type = u128;
};
