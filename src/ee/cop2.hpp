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
template<mips::CpuImpl> void vabs();
template<mips::CpuImpl> void vadd();
template<mips::CpuImpl> void vadda();
template<mips::CpuImpl> void vaddai();
template<mips::CpuImpl> void vaddaq();
template<mips::CpuImpl> void vaddaw();
template<mips::CpuImpl> void vaddax();
template<mips::CpuImpl> void vadday();
template<mips::CpuImpl> void vaddaz();
template<mips::CpuImpl> void vaddi();
template<mips::CpuImpl> void vaddq();
template<mips::CpuImpl> void vaddw();
template<mips::CpuImpl> void vaddx();
template<mips::CpuImpl> void vaddy();
template<mips::CpuImpl> void vaddz();
template<mips::CpuImpl> void vcallms();
template<mips::CpuImpl> void vcallmsr(); // called callmsr?
template<mips::CpuImpl> void vclipw();
template<mips::CpuImpl> void vdiv();
template<mips::CpuImpl> void vftoi0();
template<mips::CpuImpl> void vftoi4();
template<mips::CpuImpl> void vftoi12();
template<mips::CpuImpl> void vftoi15();
template<mips::CpuImpl> void viadd();
template<mips::CpuImpl> void viaddi();
template<mips::CpuImpl> void viand();
template<mips::CpuImpl> void vilwr();
template<mips::CpuImpl> void vior();
template<mips::CpuImpl> void visub();
template<mips::CpuImpl> void viswr();
template<mips::CpuImpl> void vitof0();
template<mips::CpuImpl> void vitof4();
template<mips::CpuImpl> void vitof12();
template<mips::CpuImpl> void vitof15();
template<mips::CpuImpl> void vlqd();
template<mips::CpuImpl> void vlqi();
template<mips::CpuImpl> void vmadd();
template<mips::CpuImpl> void vmadda();
template<mips::CpuImpl> void vmaddai();
template<mips::CpuImpl> void vmaddaq();
template<mips::CpuImpl> void vmaddaw();
template<mips::CpuImpl> void vmaddax();
template<mips::CpuImpl> void vmadday();
template<mips::CpuImpl> void vmaddaz();
template<mips::CpuImpl> void vmaddi();
template<mips::CpuImpl> void vmaddq();
template<mips::CpuImpl> void vmaddw();
template<mips::CpuImpl> void vmaddx();
template<mips::CpuImpl> void vmaddy();
template<mips::CpuImpl> void vmaddz();
template<mips::CpuImpl> void vmax();
template<mips::CpuImpl> void vmaxi();
template<mips::CpuImpl> void vmaxw();
template<mips::CpuImpl> void vmaxx();
template<mips::CpuImpl> void vmaxy();
template<mips::CpuImpl> void vmaxz();
template<mips::CpuImpl> void vmfir();
template<mips::CpuImpl> void vmini();
template<mips::CpuImpl> void vminii();
template<mips::CpuImpl> void vminiw();
template<mips::CpuImpl> void vminix();
template<mips::CpuImpl> void vminiy();
template<mips::CpuImpl> void vminiz();
template<mips::CpuImpl> void vmove();
template<mips::CpuImpl> void vmr32();
template<mips::CpuImpl> void vmsub();
template<mips::CpuImpl> void vmsubai();
template<mips::CpuImpl> void vmsubaq();
template<mips::CpuImpl> void vmsubaw();
template<mips::CpuImpl> void vmsubax();
template<mips::CpuImpl> void vmsubay();
template<mips::CpuImpl> void vmsubaz();
template<mips::CpuImpl> void vmsuba();
template<mips::CpuImpl> void vmsubi();
template<mips::CpuImpl> void vmsubq();
template<mips::CpuImpl> void vmsubw();
template<mips::CpuImpl> void vmsubx();
template<mips::CpuImpl> void vmsuby();
template<mips::CpuImpl> void vmsubz();
template<mips::CpuImpl> void vmtir();
template<mips::CpuImpl> void vmul();
template<mips::CpuImpl> void vmula();
template<mips::CpuImpl> void vmulai();
template<mips::CpuImpl> void vmulaq();
template<mips::CpuImpl> void vmulaw();
template<mips::CpuImpl> void vmulax();
template<mips::CpuImpl> void vmulay();
template<mips::CpuImpl> void vmulaz();
template<mips::CpuImpl> void vmuli();
template<mips::CpuImpl> void vmulq();
template<mips::CpuImpl> void vmulw();
template<mips::CpuImpl> void vmulx();
template<mips::CpuImpl> void vmuly();
template<mips::CpuImpl> void vmulz();
template<mips::CpuImpl> void vnop();
template<mips::CpuImpl> void vopmsub();
template<mips::CpuImpl> void vopmula();
template<mips::CpuImpl> void vrget();
template<mips::CpuImpl> void vrinit();
template<mips::CpuImpl> void vrnext();
template<mips::CpuImpl> void vrsqrt();
template<mips::CpuImpl> void vrxor();
template<mips::CpuImpl> void vsqd();
template<mips::CpuImpl> void vsqi();
template<mips::CpuImpl> void vsqrt();
template<mips::CpuImpl> void vsub();
template<mips::CpuImpl> void vsuba();
template<mips::CpuImpl> void vsubai();
template<mips::CpuImpl> void vsubaq();
template<mips::CpuImpl> void vsubaw();
template<mips::CpuImpl> void vsubax();
template<mips::CpuImpl> void vsubay();
template<mips::CpuImpl> void vsubaz();
template<mips::CpuImpl> void vsubi();
template<mips::CpuImpl> void vsubq();
template<mips::CpuImpl> void vsubw();
template<mips::CpuImpl> void vsubx();
template<mips::CpuImpl> void vsuby();
template<mips::CpuImpl> void vsubz();
template<mips::CpuImpl> void vwaitq();

} // namespace ee
