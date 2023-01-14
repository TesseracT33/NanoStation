#pragma once

#include "types.hpp"

enum class CpuImpl {
    Interpreter,
    Recompiler
};

namespace ee {
template<CpuImpl> void disassemble(u32 instr);
}
