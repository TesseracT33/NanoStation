#include "cop0.hpp"

namespace iop {

using enum mips::CpuImpl;

template<> void mfc0<Interpreter>(u32 rd, u32 rt)
{
}

template<> void mtc0<Interpreter>(u32 rd, u32 rt)
{
}

} // namespace iop
