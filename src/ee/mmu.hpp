#pragma once

#include "exceptions.hpp"
#include "types.hpp"

#include <bit>

namespace ee {
enum class Alignment {
    Aligned,
    Unaligned
};

enum class MemOp {
    DataRead,
    DataWrite,
    InstrFetch
};

template<size_t size, Alignment alignment = Alignment::Aligned, MemOp mem_op = MemOp::DataRead>
auto virtual_read(s32 addr);

template<size_t size, Alignment alignment = Alignment::Aligned> void virtual_write(s32 addr, auto data);

template<size_t size, Alignment alignment, MemOp mem_op> auto virtual_read(s32 addr)
{
    static_assert(std::has_single_bit(size) && size <= 16);
    if constexpr (alignment == Alignment::Aligned && size > 1 && size < 16 && mem_op == MemOp::DataRead) {
        if (addr & (size - 1)) {
            address_error_exception(addr, mem_op);
        }
    }
    if constexpr (size == 1) return u8{};
    if constexpr (size == 2) return u16{};
    if constexpr (size == 4) return u32{};
    if constexpr (size == 8) return u64{};
    if constexpr (size == 16) return m128i{};
}

template<size_t size, Alignment alignment> void virtual_write(s32 addr, auto data)
{
    static_assert(std::has_single_bit(size) && size <= 16 && size == sizeof(data));
    if constexpr (alignment == Alignment::Aligned && size > 1) {
        if (addr & (size - 1)) {
            address_error_exception(addr, MemOp::DataWrite);
            return;
        }
    }
}

} // namespace ee
