#include "vu.hpp"

#include <algorithm>
#include <cmath>
#include <functional>

namespace ee::vu {

using enum mips::CpuImpl;

#define BC      (instr & 3)
#define FD      (instr >> 6 & 31)
#define ID      (instr >> 6 & 31)
#define FS      (instr >> 11 & 31)
#define IS      (instr >> 11 & 31)
#define FT      (instr >> 16 & 31)
#define IT      (instr >> 16 & 31)
#define DST(i)  (instr & 1 << (24 - i))
#define INDEX_X 0
#define INDEX_Y 1
#define INDEX_Z 2
#define INDEX_W 3

using add_t = std::plus<void>;
using mul_t = std::multiplies<void>;
using sub_t = std::minus<void>;

template<typename Fun> static void arith(u32 instr, auto op2);
template<typename Fun> static void arith_acc(u32 instr, auto op2);
static EeF32 get_lane(auto val, int idx);
static void max(u32 instr, auto op2);
static void min(u32 instr, auto op2);
template<typename Fun> static void mul_and_arith(u32 instr, auto op2);
template<typename Fun> static void mul_and_arith_acc(u32 instr, auto op2);
template<typename Fun> static void outer_prod_and_arith(u32 instr);

// ADD, ADDI, ADDQ, ADDx, ADDy, ADDz, ADDw
// MUL, MULI, MULQ, MULx, MULy, MULz, MULw
// SUB, SUBI, SUBQ, SUBx, SUBy, SUBz, SUBw
template<typename Fun> void arith(u32 instr, auto op2)
{
    static const Fun fun{};
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = fun(vf[FS][i], get_lane(op2, i));
            vf.set(FT, i, result);
            mac.update(i, result);
        }
    }
}

// ADDA, ADDAI, ADDAQ, ADDAx, ADDAy, ADDAz, ADDAw
// MULA, MULAI, MULAQ, MULAx, MULAy, MULAz, MULAw
// SUBA, SUBAI, SUBAQ, SUBAx, SUBAy, SUBAz, SUBAw
template<typename Fun> void arith_acc(u32 instr, auto op2)
{
    static const Fun fun{};
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = fun(vf[FS][i], get_lane(op2, i));
            acc[i] = result;
            mac.update(i, result);
        }
    }
}

EeF32 get_lane(auto val, [[maybe_unused]] int idx)
{
    if constexpr (requires { val[idx]; }) {
        return val[idx];
    } else {
        return val;
    }
}

// MAX, MAXI, MAXx, MAXy, MAXz, MAXw
void max(u32 instr, auto op2)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = std::max(vf[FS][i], get_lane(op2, i));
            vf.set(FD, i, result);
            mac.update(i, result);
        }
    }
}

// MIN, MINI, MINx, MINy, MINz, MINw
void min(u32 instr, auto op2)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = std::min(vf[FS][i], get_lane(op2, i));
            vf.set(FD, i, result);
            mac.update(i, result);
        }
    }
}

// MADD, MADDI, MADDQ, MADDx, MADDy, MADDz, MADDw
// MSUB, MSUBI, MSUBQ, MSUBx, MSUBy, MSUBz, MSUBw
template<typename Fun> void mul_and_arith(u32 instr, auto op2)
{
    static const Fun fun{};
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = vf[FS][i] * get_lane(op2, i);
            result = fun(acc[i], result);
            vf.set(FD, i, result);
            mac.update(i, result);
        }
    }
}

// MADDA, MADDAI, MADDAQ, MADDAx, MADDAy, MADDAz, MADDAw
// MSUBA, MSUBAI, MSUBAQ, MSUBAx, MSUBAy, MSUBAz, MSUBAw
template<typename Fun> void mul_and_arith_acc(u32 instr, auto op2)
{
    static const Fun fun{};
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = vf[FS][i] * get_lane(op2, i);
            result = fun(acc[i], result);
            acc[i] = result;
            mac.update(i, result);
        }
    }
}

template<> void vabs<Interpreter>(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = std::abs(vf[FT][i]);
            vf.set(FS, i, result);
            mac.update(i, result);
        }
    }
}

template<> void vadd<Interpreter>(u32 instr)
{
    arith<add_t>(instr, vf[FS]);
}

template<> void vadda<Interpreter>(u32 instr)
{
    arith_acc<add_t>(instr, vf[FT]);
}

template<> void vaddai<Interpreter>(u32 instr)
{
    arith_acc<add_t>(instr, I);
}

template<> void vaddaq<Interpreter>(u32 instr)
{
    arith_acc<add_t>(instr, Q);
}

template<> void vaddabc<Interpreter>(u32 instr)
{
    arith_acc<add_t>(instr, vf[FT][BC]);
}

template<> void vaddi<Interpreter>(u32 instr)
{
    arith<add_t>(instr, I);
}

template<> void vaddq<Interpreter>(u32 instr)
{
    arith<add_t>(instr, Q);
}

template<> void vaddbc<Interpreter>(u32 instr)
{
    arith<add_t>(instr, vf[FS][BC]);
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
    EeF32 fs = vf[FS][instr >> 21 & 3];
    if (fs == 0.f) {
        status.signal_invalid(); // TODO: result?
    } else {
        EeF32 ft = vf[FT][instr >> 23 & 3];
        Q = ft / fs;
    }
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
    vi[ID] = vi[IS] + vi[IT];
}

template<> void viaddi<Interpreter>(u32 instr)
{
    vi[IT] = vi[IS] + (instr >> 6 & 31); // TODO: sign-extend imm?
}

template<> void viand<Interpreter>(u32 instr)
{
    vi[ID] = vi[IS] & vi[IT];
}

template<> void vilwr<Interpreter>(u32 instr)
{
}

template<> void vior<Interpreter>(u32 instr)
{
    vi[ID] = vi[IS] | vi[IT];
}

template<> void visub<Interpreter>(u32 instr)
{
    vi[ID] = vi[IS] - vi[IT];
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
    mul_and_arith<add_t>(instr, vf[FS]);
}

template<> void vmadda<Interpreter>(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, vf[FT]);
}

template<> void vmaddai<Interpreter>(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, I);
}

template<> void vmaddaq<Interpreter>(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, Q);
}

template<> void vmaddabc<Interpreter>(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, vf[FT][BC]);
}

template<> void vmaddi<Interpreter>(u32 instr)
{
    mul_and_arith<add_t>(instr, I);
}

template<> void vmaddq<Interpreter>(u32 instr)
{
    mul_and_arith<add_t>(instr, Q);
}

template<> void vmaddbc<Interpreter>(u32 instr)
{
    mul_and_arith<add_t>(instr, vf[FS][BC]);
}

template<> void vmax<Interpreter>(u32 instr)
{
    max(instr, vf[FT]);
}

template<> void vmaxi<Interpreter>(u32 instr)
{
    max(instr, I);
}

template<> void vmaxbc<Interpreter>(u32 instr)
{
    max(instr, vf[FT][BC]);
}

template<> void vmfir<Interpreter>(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            vf.set(FT, i, vi[IS]);
            mac.update(i, vi[IS]);
        }
    }
}

template<> void vmini<Interpreter>(u32 instr)
{
    min(instr, vf[FT]);
}

template<> void vminii<Interpreter>(u32 instr)
{
    min(instr, I);
}

template<> void vminibc<Interpreter>(u32 instr)
{
    min(instr, vf[FT][BC]);
}

template<> void vmove<Interpreter>(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            vf.set(FT, i, vf[FS][i]);
            mac.update(i, vf[FS][i]);
        }
    }
}

template<> void vmr32<Interpreter>(u32 instr)
{
}

template<> void vmsub<Interpreter>(u32 instr)
{
    mul_and_arith<sub_t>(instr, vf[FS]);
}

template<> void vmsuba<Interpreter>(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, vf[FT]);
}

template<> void vmsubai<Interpreter>(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, I);
}

template<> void vmsubaq<Interpreter>(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, Q);
}

template<> void vmsubabc<Interpreter>(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, vf[FT][BC]);
}

template<> void vmsubi<Interpreter>(u32 instr)
{
    mul_and_arith<sub_t>(instr, I);
}

template<> void vmsubq<Interpreter>(u32 instr)
{
    mul_and_arith<sub_t>(instr, Q);
}

template<> void vmsubbc<Interpreter>(u32 instr)
{
    mul_and_arith<sub_t>(instr, vf[FS][BC]);
}

template<> void vmtir<Interpreter>(u32 instr)
{
    vi[IT] = std::bit_cast<u32>(vf[FS][instr >> 21 & 3]);
}

template<> void vmul<Interpreter>(u32 instr)
{
    arith<mul_t>(instr, vf[FS]);
}

template<> void vmula<Interpreter>(u32 instr)
{
    arith_acc<mul_t>(instr, vf[FT]);
}

template<> void vmulai<Interpreter>(u32 instr)
{
    arith_acc<mul_t>(instr, I);
}

template<> void vmulaq<Interpreter>(u32 instr)
{
    arith_acc<mul_t>(instr, Q);
}

template<> void vmulabc<Interpreter>(u32 instr)
{
    arith_acc<mul_t>(instr, vf[FT][BC]);
}

template<> void vmuli<Interpreter>(u32 instr)
{
    arith<mul_t>(instr, I);
}

template<> void vmulq<Interpreter>(u32 instr)
{
    arith<mul_t>(instr, Q);
}

template<> void vmulbc<Interpreter>(u32 instr)
{
    arith<mul_t>(instr, vf[FS][BC]);
}

template<> void vnop<Interpreter>(u32 instr)
{
}

template<> void vopmsub<Interpreter>(u32 instr)
{
    EeF32 a = vf[FS][INDEX_Y] * vf[FT][INDEX_Z];
    EeF32 b = vf[FS][INDEX_Z] * vf[FT][INDEX_Y];
    EeF32 c = vf[FS][INDEX_Z] * vf[FT][INDEX_X];
    EeF32 d = vf[FS][INDEX_X] * vf[FT][INDEX_Z];
    EeF32 e = vf[FS][INDEX_X] * vf[FT][INDEX_Y];
    EeF32 f = vf[FS][INDEX_Y] * vf[FT][INDEX_X];
    vf.set(FD, INDEX_X, a - b);
    vf.set(FD, INDEX_Y, c - d);
    vf.set(FD, INDEX_Z, e - f);
    mac.update(INDEX_X, a - b);
    mac.update(INDEX_Y, c - d);
    mac.update(INDEX_Z, e - f);
}

template<> void vopmula<Interpreter>(u32 instr)
{
    EeF32 a = vf[FS][INDEX_Y] * vf[FT][INDEX_Z];
    EeF32 b = vf[FS][INDEX_Z] * vf[FT][INDEX_Y];
    EeF32 c = vf[FS][INDEX_Z] * vf[FT][INDEX_X];
    EeF32 d = vf[FS][INDEX_X] * vf[FT][INDEX_Z];
    EeF32 e = vf[FS][INDEX_X] * vf[FT][INDEX_Y];
    EeF32 f = vf[FS][INDEX_Y] * vf[FT][INDEX_X];
    acc[INDEX_X] = a + b;
    acc[INDEX_Y] = c + d;
    acc[INDEX_Z] = e + f;
}

template<> void vrget<Interpreter>(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            vf.set(FT, i, R);
            mac.update(i, R);
        }
    }
}

template<> void vrinit<Interpreter>(u32 instr)
{
}

template<> void vrnext<Interpreter>(u32 instr)
{
}

template<> void vrsqrt<Interpreter>(u32 instr)
{
    EeF32 op1 = vf[FS][instr >> 21 & 3];
    if (op1 > 0.f) {
        EeF32 sqrt = std::sqrt(op1);
        EeF32 op2 = vf[FT][instr >> 23 & 3];
        Q = op2 / sqrt;
    } else {
        status.signal_invalid();
    }
}

template<> void vrxor<Interpreter>(u32 instr)
{
    R = R ^ vf[FS][instr >> 21 & 3];
}

template<> void vsqd<Interpreter>(u32 instr)
{
}

template<> void vsqi<Interpreter>(u32 instr)
{
}

template<> void vsqrt<Interpreter>(u32 instr)
{
    EeF32 op = vf[FT][instr >> 23 & 3];
    if (op >= 0.f) {
        Q = std::sqrt(op);
    } else {
        status.signal_invalid();
    }
}

template<> void vsub<Interpreter>(u32 instr)
{
    arith<sub_t>(instr, vf[FS]);
}

template<> void vsuba<Interpreter>(u32 instr)
{
    arith_acc<sub_t>(instr, vf[FT]);
}

template<> void vsubai<Interpreter>(u32 instr)
{
    arith_acc<sub_t>(instr, I);
}

template<> void vsubaq<Interpreter>(u32 instr)
{
    arith_acc<sub_t>(instr, Q);
}

template<> void vsubabc<Interpreter>(u32 instr)
{
    arith_acc<sub_t>(instr, vf[FT][BC]);
}

template<> void vsubi<Interpreter>(u32 instr)
{
    arith<sub_t>(instr, I);
}

template<> void vsubq<Interpreter>(u32 instr)
{
    arith<sub_t>(instr, Q);
}

template<> void vsubbc<Interpreter>(u32 instr)
{
    arith<sub_t>(instr, vf[FS][BC]);
}

template<> void vwaitq<Interpreter>(u32 instr)
{
}

} // namespace ee::vu
