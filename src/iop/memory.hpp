#pragma once

#include "types.hpp"

#include <array>
#include <concepts>

namespace iop {

template<typename T>
concept IopUInt = std::unsigned_integral<T> && sizeof(T) <= 4;

enum class Alignment {
    Aligned,
    Unaligned
};

enum class MemOp {
    DataRead,
    DataWrite,
    InstrFetch
};

inline constexpr size_t bios_size = 512 * 1024;

inline std::array<u8, bios_size> bios;

template<IopUInt Int, Alignment alignment = Alignment::Aligned, MemOp mem_op = MemOp::DataRead> Int read(u32 addr);

template<size_t size, Alignment alignment = Alignment::Aligned> void write(u32 addr, auto data);

} // namespace iop
