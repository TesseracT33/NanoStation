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

#if (defined __clang__ || defined __GNUC__) && defined __SIZEOF_INT128__
#define INT128_AVAILABLE 1
using u128 = __uint128_t;
using s128 = __int128_t;
#endif

using uint = unsigned;

using std::size_t;
