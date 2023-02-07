#include "memory.hpp"
#include "exceptions.hpp"

namespace iop {

template<IopUInt Int, Alignment alignment, MemOp mem_op> Int read(u32 addr)
{
    static constexpr size_t size = sizeof(Int);
    if constexpr (alignment == Alignment::Aligned && size > 1 && mem_op == MemOp::DataRead) {
        if (addr & (size - 1)) {
            address_error_exception(addr, mem_op);
            return {};
        }
    }

    return {};
}

template<size_t size, Alignment alignment> void write(u32 addr, auto data)
{
}

template u8 read<u8, Alignment::Aligned, MemOp::DataRead>(u32);
template u16 read<u16, Alignment::Aligned, MemOp::DataRead>(u32);
template u32 read<u32, Alignment::Aligned, MemOp::DataRead>(u32);
template u32 read<u32, Alignment::Unaligned, MemOp::DataRead>(u32);
template u32 read<u32, Alignment::Aligned, MemOp::InstrFetch>(u32);

template void write<1, Alignment::Aligned>(u32, u8);
template void write<1, Alignment::Aligned>(u32, s8);
template void write<2, Alignment::Aligned>(u32, u16);
template void write<2, Alignment::Aligned>(u32, s16);
template void write<4, Alignment::Aligned>(u32, u32);
template void write<4, Alignment::Aligned>(u32, s32);
template void write<4, Alignment::Unaligned>(u32, u32);
template void write<4, Alignment::Unaligned>(u32, s32);

} // namespace iop
