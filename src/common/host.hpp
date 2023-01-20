#pragma once

#include <bit>

#if defined __x86_64__ || defined _M_X64
struct {
    static constexpr bool x64 = 1;
    static constexpr bool arm64 = 0;
} constexpr arch;
#define X64 1
#elif defined __aarch64__ || defined _M_ARM64
struct {
    static constexpr bool x64 = 0;
    static constexpr bool arm64 = 1;
} constexpr arch;
#define ARM64 1
#else
#error Unsupported architecture; only x86-64 and arm64 are supported.
#endif

static_assert(std::endian::native == std::endian::little, "Only little-endian hosts are supported.");