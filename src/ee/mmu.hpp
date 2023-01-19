#pragma once

#include "types.hpp"

namespace ee {
enum class MemOp {
    DataRead,
    DataWrite,
    InstrFetch
};

template<typename T> T virtual_read(s32 addr)
{
    return T{};
}

template<size_t num_bytes> void virtual_write(s32 addr, auto data)
{

}

}