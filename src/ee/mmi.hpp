#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

namespace ee {
template<mips::CpuImpl> void madd(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void madd1(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void maddu(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void maddu1(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pabsh(u32 rt, u32 rd);
template<mips::CpuImpl> void pabsw(u32 rt, u32 rd);
template<mips::CpuImpl> void paddb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void paddh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void paddsb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void paddsh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void paddsw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void paddub(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void padduh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void padduw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void paddw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void padsbh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pand(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pceqb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pceqh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pceqw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pcgtb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pcgth(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pcgtw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pcpyh(u32 rt, u32 rd);
template<mips::CpuImpl> void pcpyld(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pcpyud(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pdivbw(u32 rs, u32 rt);
template<mips::CpuImpl> void pdivuw(u32 rs, u32 rt);
template<mips::CpuImpl> void pdivw(u32 rs, u32 rt);
template<mips::CpuImpl> void pexch(u32 rt, u32 rd);
template<mips::CpuImpl> void pexcw(u32 rt, u32 rd);
template<mips::CpuImpl> void pexeh(u32 rt, u32 rd);
template<mips::CpuImpl> void pexew(u32 rt, u32 rd);
template<mips::CpuImpl> void pext5(u32 rt, u32 rd);
template<mips::CpuImpl> void pextlb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pextlh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pextlw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pextub(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pextuh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pextuw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void phmadh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void phmsbh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pinteh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pinth(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void plzcw(u32 rs, u32 rd);
template<mips::CpuImpl> void pmaddh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmadduw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmaddw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmaxh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmaxw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmfhi(u32 rd);
template<mips::CpuImpl> void pmfhl(u32 rd, u32 fmt);
template<mips::CpuImpl> void pmflo(u32 rd);
template<mips::CpuImpl> void pminh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pminw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmsubh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmsubw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmthi(u32 rs);
template<mips::CpuImpl> void pmthl(u32 rs, u32 fmt);
template<mips::CpuImpl> void pmtlo(u32 rs);
template<mips::CpuImpl> void pmulth(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmultuw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pmultw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pnor(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void por(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void ppac5(u32 rt, u32 rd);
template<mips::CpuImpl> void ppacb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void ppach(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void ppacw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void prevh(u32 rt, u32 rd);
template<mips::CpuImpl> void prot3w(u32 rt, u32 rd);
template<mips::CpuImpl> void psllh(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void psllvw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psllw(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void psrah(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void psravw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psraw(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void psrlh(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void psrlvw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psrlw(u32 rt, u32 rd, u32 sa);
template<mips::CpuImpl> void psubb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubsb(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubsh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubsw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubub(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubuh(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubuw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void psubw(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void pxor(u32 rs, u32 rt, u32 rd);
template<mips::CpuImpl> void qfsrv(u32 rs, u32 rt, u32 rd);
} // namespace ee
