#include "mmi.hpp"

// reference: https://wiki.qemu.org/File:C790.pdf

namespace ee {
void madd(u32 rs, u32 rt, u32 rd) // Multiply/Add
{
}

void madd1(u32 rs, u32 rt, u32 rd) // Multiply-Add Pipeline 1
{
}

void maddu(u32 rs, u32 rt, u32 rd) // Multiply/Add Unsigned
{
}

void maddu1(u32 rs, u32 rt, u32 rd) // Multiply-Add Unsigned Pipeline 1
{
}

void pabsh(u32 rt, u32 rd) // Parallel Absolute Halfword
{
}

void pabsw(u32 rt, u32 rd) // Parallel Absolute Word
{
}

void paddb(u32 rs, u32 rt, u32 rd) // Parallel Add Byte
{
}

void paddh(u32 rs, u32 rt, u32 rd) // Parallel Add Halfword
{
}

void paddsb(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Byte
{
}

void paddsh(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Halfword
{
}

void paddsw(u32 rs, u32 rt, u32 rd) // Parallel Add with Signed Saturation Word
{
}

void paddub(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Byte
{
}

void padduh(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Halfword
{
}

void padduw(u32 rs, u32 rt, u32 rd) // Parallel Add with Unsigned saturation Word
{
}

void paddw(u32 rs, u32 rt, u32 rd) // Parallel Add Word
{
}

void padsbh(u32 rs, u32 rt, u32 rd) // Parallel Add/Subtract Halfword
{
}

void pand(u32 rs, u32 rt, u32 rd) // Parallel AND
{
}

void pceqb(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Byte
{
}

void pceqh(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Halfword
{
}

void pceqw(u32 rs, u32 rt, u32 rd) // Parallel Compare for Equal Word
{
}

void pcgtb(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Byte
{
}

void pcgth(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Halfword
{
}

void pcgtw(u32 rs, u32 rt, u32 rd) // Parallel Compare for Greater Than Word
{
}

void pcpyh(u32 rt, u32 rd) // Parallel Copy Halfword
{
}

void pcpyld(u32 rs, u32 rt, u32 rd) // Parallel Copy Lower Doubleword
{
}

void pcpyud(u32 rs, u32 rt, u32 rd) // Parallel Copy Upper Doubleword
{
}

void pdivbw(u32 rs, u32 rt) // Parallel Divide Broadcast Word
{
}

void pdivuw(u32 rs, u32 rt) // Parallel Divide Unsigned Word
{
}

void pdivw(u32 rs, u32 rt) // Parallel Divide Word
{
}

void pexch(u32 rt, u32 rd) // Parallel Exchange Center Halfword
{
}

void pexcw(u32 rt, u32 rd) // Parallel Exchange Center Word
{
}

void pexeh(u32 rt, u32 rd) // Parallel Exchange Even Halfword
{
}

void pexew(u32 rt, u32 rd) // Parallel Exchange Even Word
{
}

void pext5(u32 rt, u32 rd) // Parallel Extend Upper from 5 bits
{
}

void pextlb(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Byte
{
}

void pextlh(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Halfword
{
}

void pextlw(u32 rs, u32 rt, u32 rd) // Parallel Extend Lower from Word
{
}

void pextub(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Byte
{
}

void pextuh(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Halfword
{
}

void pextuw(u32 rs, u32 rt, u32 rd) // Parallel Extend Upper from Word
{
}

void phmadh(u32 rs, u32 rt, u32 rd) // Parallel Horizontal Multiply-Add Halfword
{
}

void phmsbh(u32 rs, u32 rt, u32 rd) // Parallel Horizontal Multiply-Subtract Halfword
{
}

void pinteh(u32 rs, u32 rt, u32 rd) // Parallel Interleave Even Halfword
{
}

void pinth(u32 rs, u32 rt, u32 rd) // Parallel Interleave Halfword
{
}

void plzcw(u32 rs, u32 rd) // Parallel Leading Zero or One Count Word
{
}

void pmaddh(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Halfword
{
}

void pmadduw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Unsigned Word
{
}

void pmaddw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Add Word
{
}

void pmaxh(u32 rs, u32 rt, u32 rd) // Parallel Maximum Halfword
{
}

void pmaxw(u32 rs, u32 rt, u32 rd) //  Parallel Maximum Word
{
}

void pmfhi(u32 rd) //  Parallel Move From HI Register
{
}

void pmfhl(u32 rd, u32 fmt) // Parallel Move From HI/LO Register
{
}

void pmflo(u32 rd) // Parallel Move From LO Register
{
}

void pminh(u32 rs, u32 rt, u32 rd) // Parallel Minimum Halfword
{
}

void pminw(u32 rs, u32 rt, u32 rd) // Parallel Minimum Word
{
}

void pmsubh(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Subtract Halfword
{
}

void pmsubw(u32 rs, u32 rt, u32 rd) // Parallel Multiply-Subtract Word
{
}

void pmthi(u32 rs) // Parallel Move To HI Register
{
}

void pmthl(u32 rs, u32 fmt) // Parallel Move To HI/LO Register
{
}

void pmtlo(u32 rs) // Parallel Move To LO Register
{
}

void pmulth(u32 rs, u32 rt, u32 rd) // Parallel Multiply Halfword
{
}

void pmultuw(u32 rs, u32 rt, u32 rd) // Parallel Multiply Unsigned Word
{
}

void pmultw(u32 rs, u32 rt, u32 rd) // Parallel Multiply Word
{
}

void pnor(u32 rs, u32 rt, u32 rd) // Parallel NOR
{
}

void por(u32 rs, u32 rt, u32 rd) // Parallel OR
{
}

void ppac5(u32 rt, u32 rd) // Parallel Pack to 5 bits
{
}

void ppacb(u32 rs, u32 rt, u32 rd) // Parallel Pack to Byte
{
}

void ppach(u32 rs, u32 rt, u32 rd) // Parallel Pack to Halfword
{
}

void ppacw(u32 rs, u32 rt, u32 rd) // Parallel Pack to Word
{
}

void prevh(u32 rt, u32 rd) // Parallel Reverse Halfword
{
}

void prot3w(u32 rt, u32 rd) // Parallel Rotate 3 Words
{
}

void psllh(u32 rt, u32 rd, u32 sa) // Parallel Shift Left Logical Halfword
{
}

void psllvw(u32 rs, u32 rt, u32 rd) // Parallel Shift Left Logical Variable Word
{
}

void psllw(u32 rt, u32 rd, u32 sa) // Parallel Shift Left Logical Word
{
}

void psrah(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Arithmetic Halfword
{
}

void psravw(u32 rs, u32 rt, u32 rd) // Parallel Shift Right Arithmetic Variable Word
{
}

void psraw(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Arithmetic Word
{
}

void psrlh(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Logical Halfword
{
}

void psrlvw(u32 rs, u32 rt, u32 rd) // Parallel Shift Right Logical Variable Word
{
}

void psrlw(u32 rt, u32 rd, u32 sa) // Parallel Shift Right Logical Word
{
}

void psubb(u32 rs, u32 rt, u32 rd) // Parallel Subtract Byte
{
}

void psubh(u32 rs, u32 rt, u32 rd) // Parallel Subtract Halfword
{
}

void psubsb(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Byte
{
}

void psubsh(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Halfword
{
}

void psubsw(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Signed Saturation Word
{
}

void psubub(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Byte
{
}

void psubuh(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Halfword
{
}

void psubuw(u32 rs, u32 rt, u32 rd) // Parallel Subtract with Unsigned saturation Word
{
}

void psubw(u32 rs, u32 rt, u32 rd) // Parallel Subtract Word
{
}

void pxor(u32 rs, u32 rt, u32 rd) // Parallel XOR
{
}

void qfsrv(u32 rs, u32 rt, u32 rd) // Quadword Funnel Shift Right Variable
{
}

} // namespace ee
