#pragma once

#include "mips/mips.hpp"

namespace ee {

template<mips::CpuImpl> void bc2f();
template<mips::CpuImpl> void bc2fl();
template<mips::CpuImpl> void bc2t();
template<mips::CpuImpl> void bc2tl();
template<mips::CpuImpl> void cfc2();
template<mips::CpuImpl> void ctc2();
template<mips::CpuImpl> void lqc2();
template<mips::CpuImpl> void qmfc2();
template<mips::CpuImpl> void qmtc2();
template<mips::CpuImpl> void sqc2();

} // namespace ee
