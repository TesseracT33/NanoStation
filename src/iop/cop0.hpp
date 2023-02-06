#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

namespace iop {
template<mips::CpuImpl> void mfc0(u32 rd, u32 rt);
template<mips::CpuImpl> void mtc0(u32 rd, u32 rt);
} // namespace iop
