#include "cop2.hpp"

namespace ee {

using enum mips::CpuImpl;

template<> void bc2f<Interpreter>()
{
}

template<> void bc2fl<Interpreter>()
{
}

template<> void bc2t<Interpreter>()
{
}

template<> void bc2tl<Interpreter>()
{
}

template<> void cfc2<Interpreter>()
{
}

template<> void ctc2<Interpreter>()
{
}

template<> void lqc2<Interpreter>()
{
}

template<> void qmfc2<Interpreter>()
{
}

template<> void qmtc2<Interpreter>()
{
}

template<> void sqc2<Interpreter>()
{
}

} // namespace ee
