#pragma once

#include "types.hpp"

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

template<typename T, Alignment alignment = Alignment::Aligned> T virtual_read(s32 addr)
{
    return T{};
}

template<size_t num_bytes, Alignment alignment = Alignment::Aligned> void virtual_write(s32 addr, auto data)
{

}

}