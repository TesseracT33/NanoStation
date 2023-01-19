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

using uint = unsigned;

struct u128 {
    // TODO
    s8 s8() const { return 0; }
    u8 u8() const { return 0; }
    s16 s16() const { return 0; }
    u16 u16() const { return 0; }
    s32 s32() const { return 0; }
    u32 u32() const { return 0; }
    s64 s64() const { return 0; }
    u64 u64() const { return 0; }
};

struct s128 {
    // TODO
};

using std::size_t;
