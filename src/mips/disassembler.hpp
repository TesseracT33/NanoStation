#pragma once

#include "mips.hpp"
#include "numtypes.hpp"

#include <string>

namespace mips {

template<CpuImpl> void disassemble_ee(u32 instr);
template<CpuImpl> void disassemble_iop(u32 instr);
std::string disassemble_str(u32 instr);

} // namespace mips
