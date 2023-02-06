#pragma once

#include "types.hpp"

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

template<IopUInt Int, Alignment alignment = Alignment::Aligned, MemOp mem_op = MemOp::DataRead> Int read(u32 addr);

template<size_t size, Alignment alignment = Alignment::Aligned> void write(u32 addr, auto data);

} // namespace iop
