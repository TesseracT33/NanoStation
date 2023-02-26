#include "vu.hpp"

#include <algorithm>

namespace ee::vu {

using enum mips::CpuImpl;

#define BC     (instr & 3)
#define FD     (instr >> 11 & 31)
#define FS     (instr >> 11 & 31)
#define FT     (instr >> 16 & 31)
#define DST(i) (instr & 1 << (21 + i))

template<> void vabs<Interpreter>(u32 instr)
{
}

template<> void vadd<Interpreter>(u32 instr)
{
}

template<> void vadda<Interpreter>(u32 instr)
{
}

template<> void vaddai<Interpreter>(u32 instr)
{
}

template<> void vaddaq<Interpreter>(u32 instr)
{
}

template<> void vaddabc<Interpreter>(u32 instr)
{
}

template<> void vaddi<Interpreter>(u32 instr)
{
}

template<> void vaddq<Interpreter>(u32 instr)
{
}

template<> void vaddbc<Interpreter>(u32 instr)
{
}

template<> void vcallms<Interpreter>(u32 instr)
{
}

template<> void vcallmsr<Interpreter>(u32 instr)
{
}

template<> void vclipw<Interpreter>(u32 instr)
{
}

template<> void vdiv<Interpreter>(u32 instr)
{
}

template<> void vftoi0<Interpreter>(u32 instr)
{
}

template<> void vftoi4<Interpreter>(u32 instr)
{
}

template<> void vftoi12<Interpreter>(u32 instr)
{
}

template<> void vftoi15<Interpreter>(u32 instr)
{
}

template<> void viadd<Interpreter>(u32 instr)
{
}

template<> void viaddi<Interpreter>(u32 instr)
{
}

template<> void viand<Interpreter>(u32 instr)
{
}

template<> void vilwr<Interpreter>(u32 instr)
{
}

template<> void vior<Interpreter>(u32 instr)
{
}

template<> void visub<Interpreter>(u32 instr)
{
}

template<> void viswr<Interpreter>(u32 instr)
{
}

template<> void vitof0<Interpreter>(u32 instr)
{
}

template<> void vitof4<Interpreter>(u32 instr)
{
}

template<> void vitof12<Interpreter>(u32 instr)
{
}

template<> void vitof15<Interpreter>(u32 instr)
{
}

template<> void vlqd<Interpreter>(u32 instr)
{
}

template<> void vlqi<Interpreter>(u32 instr)
{
}

template<> void vmadd<Interpreter>(u32 instr)
{
}

template<> void vmadda<Interpreter>(u32 instr)
{
}

template<> void vmaddai<Interpreter>(u32 instr)
{
}

template<> void vmaddaq<Interpreter>(u32 instr)
{
}

template<> void vmaddabc<Interpreter>(u32 instr)
{
}

template<> void vmaddi<Interpreter>(u32 instr)
{
}

template<> void vmaddq<Interpreter>(u32 instr)
{
}

template<> void vmaddbc<Interpreter>(u32 instr)
{
}

template<> void vmax<Interpreter>(u32 instr)
{
}

template<> void vmaxi<Interpreter>(u32 instr)
{
}

template<> void vmaxbc<Interpreter>(u32 instr)
{
}

template<> void vmfir<Interpreter>(u32 instr)
{
}

template<> void vmini<Interpreter>(u32 instr)
{
}

template<> void vminii<Interpreter>(u32 instr)
{
}

template<> void vminibc<Interpreter>(u32 instr)
{
}

template<> void vmove<Interpreter>(u32 instr)
{
}

template<> void vmr32<Interpreter>(u32 instr)
{
}

template<> void vmsub<Interpreter>(u32 instr)
{
}

template<> void vmsubai<Interpreter>(u32 instr)
{
}

template<> void vmsubaq<Interpreter>(u32 instr)
{
}

template<> void vmsubabc<Interpreter>(u32 instr)
{
}

template<> void vmsuba<Interpreter>(u32 instr)
{
}

template<> void vmsubi<Interpreter>(u32 instr)
{
}

template<> void vmsubq<Interpreter>(u32 instr)
{
}

template<> void vmsubbc<Interpreter>(u32 instr)
{
}

template<> void vmtir<Interpreter>(u32 instr)
{
}

template<> void vmul<Interpreter>(u32 instr)
{
}

template<> void vmula<Interpreter>(u32 instr)
{
}

template<> void vmulai<Interpreter>(u32 instr)
{
}

template<> void vmulaq<Interpreter>(u32 instr)
{
}

template<> void vmulabc<Interpreter>(u32 instr)
{
}

template<> void vmuli<Interpreter>(u32 instr)
{
}

template<> void vmulq<Interpreter>(u32 instr)
{
}

template<> void vmulbc<Interpreter>(u32 instr)
{
}

template<> void vnop<Interpreter>(u32 instr)
{
}

template<> void vopmsub<Interpreter>(u32 instr)
{
}

template<> void vopmula<Interpreter>(u32 instr)
{
}

template<> void vrget<Interpreter>(u32 instr)
{
}

template<> void vrinit<Interpreter>(u32 instr)
{
}

template<> void vrnext<Interpreter>(u32 instr)
{
}

template<> void vrsqrt<Interpreter>(u32 instr)
{
}

template<> void vrxor<Interpreter>(u32 instr)
{
}

template<> void vsqd<Interpreter>(u32 instr)
{
}

template<> void vsqi<Interpreter>(u32 instr)
{
}

template<> void vsqrt<Interpreter>(u32 instr)
{
}

template<> void vsub<Interpreter>(u32 instr)
{
}

template<> void vsuba<Interpreter>(u32 instr)
{
}

template<> void vsubai<Interpreter>(u32 instr)
{
}

template<> void vsubaq<Interpreter>(u32 instr)
{
}

template<> void vsubabc<Interpreter>(u32 instr)
{
}

template<> void vsubi<Interpreter>(u32 instr)
{
}

template<> void vsubq<Interpreter>(u32 instr)
{
}

template<> void vsubbc<Interpreter>(u32 instr)
{
}

template<> void vwaitq<Interpreter>(u32 instr)
{
}

} // namespace ee::vu
