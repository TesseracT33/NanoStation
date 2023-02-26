#include "vu.hpp"

#include <algorithm>

namespace ee::vu {

using enum mips::CpuImpl;

#define BC    (instr & 3)
#define FD    (instr >> 11 & 31)
#define FS    (instr >> 11 & 31)
#define FT    (instr >> 16 & 31)
#define DST   (instr >> 21 & 15)
#define DST_W (instr & 1 << 21)
#define DST_Z (instr & 1 << 22)
#define DST_Y (instr & 1 << 23)
#define DST_X (instr & 1 << 24)

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

template<> void vaddaw<Interpreter>(u32 instr)
{
}

template<> void vaddax<Interpreter>(u32 instr)
{
}

template<> void vadday<Interpreter>(u32 instr)
{
}

template<> void vaddaz<Interpreter>(u32 instr)
{
}

template<> void vaddi<Interpreter>(u32 instr)
{
}

template<> void vaddq<Interpreter>(u32 instr)
{
}

template<> void vaddw<Interpreter>(u32 instr)
{
}

template<> void vaddx<Interpreter>(u32 instr)
{
}

template<> void vaddy<Interpreter>(u32 instr)
{
}

template<> void vaddz<Interpreter>(u32 instr)
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

template<> void vmaddaw<Interpreter>(u32 instr)
{
}

template<> void vmaddax<Interpreter>(u32 instr)
{
}

template<> void vmadday<Interpreter>(u32 instr)
{
}

template<> void vmaddaz<Interpreter>(u32 instr)
{
}

template<> void vmaddi<Interpreter>(u32 instr)
{
}

template<> void vmaddq<Interpreter>(u32 instr)
{
}

template<> void vmaddw<Interpreter>(u32 instr)
{
}

template<> void vmaddx<Interpreter>(u32 instr)
{
}

template<> void vmaddy<Interpreter>(u32 instr)
{
}

template<> void vmaddz<Interpreter>(u32 instr)
{
}

template<> void vmax<Interpreter>(u32 instr)
{
}

template<> void vmaxi<Interpreter>(u32 instr)
{
}

template<> void vmaxw<Interpreter>(u32 instr)
{
}

template<> void vmaxx<Interpreter>(u32 instr)
{
}

template<> void vmaxy<Interpreter>(u32 instr)
{
}

template<> void vmaxz<Interpreter>(u32 instr)
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

template<> void vminiw<Interpreter>(u32 instr)
{
}

template<> void vminix<Interpreter>(u32 instr)
{
}

template<> void vminiy<Interpreter>(u32 instr)
{
}

template<> void vminiz<Interpreter>(u32 instr)
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

template<> void vmsubaw<Interpreter>(u32 instr)
{
}

template<> void vmsubax<Interpreter>(u32 instr)
{
}

template<> void vmsubay<Interpreter>(u32 instr)
{
}

template<> void vmsubaz<Interpreter>(u32 instr)
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

template<> void vmsubw<Interpreter>(u32 instr)
{
}

template<> void vmsubx<Interpreter>(u32 instr)
{
}

template<> void vmsuby<Interpreter>(u32 instr)
{
}

template<> void vmsubz<Interpreter>(u32 instr)
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

template<> void vmulaw<Interpreter>(u32 instr)
{
}

template<> void vmulax<Interpreter>(u32 instr)
{
}

template<> void vmulay<Interpreter>(u32 instr)
{
}

template<> void vmulaz<Interpreter>(u32 instr)
{
}

template<> void vmuli<Interpreter>(u32 instr)
{
}

template<> void vmulq<Interpreter>(u32 instr)
{
}

template<> void vmulw<Interpreter>(u32 instr)
{
}

template<> void vmulx<Interpreter>(u32 instr)
{
}

template<> void vmuly<Interpreter>(u32 instr)
{
}

template<> void vmulz<Interpreter>(u32 instr)
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

template<> void vsubaw<Interpreter>(u32 instr)
{
}

template<> void vsubax<Interpreter>(u32 instr)
{
}

template<> void vsubay<Interpreter>(u32 instr)
{
}

template<> void vsubaz<Interpreter>(u32 instr)
{
}

template<> void vsubi<Interpreter>(u32 instr)
{
}

template<> void vsubq<Interpreter>(u32 instr)
{
}

template<> void vsubw<Interpreter>(u32 instr)
{
}

template<> void vsubx<Interpreter>(u32 instr)
{
}

template<> void vsuby<Interpreter>(u32 instr)
{
}

template<> void vsubz<Interpreter>(u32 instr)
{
}

template<> void vwaitq<Interpreter>(u32 instr)
{
}

} // namespace ee::vu
