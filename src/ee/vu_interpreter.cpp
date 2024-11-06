#include "vu.hpp"

#include <algorithm>
#include <cmath>
#include <functional>

namespace ee::vu {

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

struct {
    bool zero[4]; // x, y, z, w
    bool sign[4];
    bool underflow[4];
    bool overflow[4];
    u16 to_u16() const
    {
        u16 ret{};
        for (int i = 0; i < 4; ++i) {
            ret |= zero[3 - i] << i;
            ret |= sign[3 - i] << (i + 4);
            ret |= underflow[3 - i] << (i + 8);
            ret |= overflow[3 - i] << (i + 12);
        }
        return ret;
    }
    void update(int dst, EeF32 result)
    {
        s32 result_s32 = std::bit_cast<s32>(result);
        s32 exponent = result_s32 & 0x7F80'0000;
        zero[dst] = result == 0.f;
        sign[dst] = result_s32 < 0;
        underflow[dst] = exponent == 0;
        overflow[dst] = exponent == 0x7F80'0000;
    }
} static mac;

struct {
    bool zero, sign, underflow, overflow, invalid, div_zero, sticky_zero, sticky_sign, sticky_underflow,
      sticky_overflow, sticky_invalid, sticky_div_zero;
    void signal_invalid()
    {
        invalid = 1;
        sticky_invalid |= 1;
    }
    u16 to_u16() const
    {
        return zero | sign << 1 | underflow << 2 | overflow << 3 | invalid << 4 | div_zero << 5 | sticky_zero << 6
             | sticky_sign << 7 | sticky_underflow << 8 | sticky_overflow << 9 | sticky_invalid << 10
             | sticky_div_zero << 11;
    }
} static status;

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
    static Fun const fun{};
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
    static Fun const fun{};
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
    static Fun const fun{};
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
    static Fun const fun{};
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = vf[FS][i] * get_lane(op2, i);
            result = fun(acc[i], result);
            acc[i] = result;
            mac.update(i, result);
        }
    }
}

void vabs(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            EeF32 result = std::abs(vf[FT][i]);
            vf.set(FS, i, result);
            mac.update(i, result);
        }
    }
}

void vadd(u32 instr)
{
    arith<add_t>(instr, vf[FS]);
}

void vadda(u32 instr)
{
    arith_acc<add_t>(instr, vf[FT]);
}

void vaddai(u32 instr)
{
    arith_acc<add_t>(instr, I);
}

void vaddaq(u32 instr)
{
    arith_acc<add_t>(instr, Q);
}

void vaddabc(u32 instr)
{
    arith_acc<add_t>(instr, vf[FT][BC]);
}

void vaddi(u32 instr)
{
    arith<add_t>(instr, I);
}

void vaddq(u32 instr)
{
    arith<add_t>(instr, Q);
}

void vaddbc(u32 instr)
{
    arith<add_t>(instr, vf[FS][BC]);
}

void vcallms(u32 instr)
{
    (void)instr;
}

void vcallmsr(u32 instr)
{
    (void)instr;
}

void vclipw(u32 instr)
{
    (void)instr;
}

void vdiv(u32 instr)
{
    EeF32 fs = vf[FS][instr >> 21 & 3];
    if (fs == 0.f) {
        status.signal_invalid(); // TODO: result?
    } else {
        EeF32 ft = vf[FT][instr >> 23 & 3];
        Q = ft / fs;
    }
}

void vftoi0(u32 instr)
{
    (void)instr;
}

void vftoi4(u32 instr)
{
    (void)instr;
}

void vftoi12(u32 instr)
{
    (void)instr;
}

void vftoi15(u32 instr)
{
    (void)instr;
}

void viadd(u32 instr)
{
    vi[ID] = vi[IS] + vi[IT];
}

void viaddi(u32 instr)
{
    vi[IT] = vi[IS] + (instr >> 6 & 31); // TODO: sign-extend imm?
}

void viand(u32 instr)
{
    vi[ID] = vi[IS] & vi[IT];
}

void vilwr(u32 instr)
{
    (void)instr;
}

void vior(u32 instr)
{
    vi[ID] = vi[IS] | vi[IT];
}

void visub(u32 instr)
{
    vi[ID] = vi[IS] - vi[IT];
}

void viswr(u32 instr)
{
    (void)instr;
}

void vitof0(u32 instr)
{
    (void)instr;
}

void vitof4(u32 instr)
{
    (void)instr;
}

void vitof12(u32 instr)
{
    (void)instr;
}

void vitof15(u32 instr)
{
    (void)instr;
}

void vlqd(u32 instr)
{
    (void)instr;
}

void vlqi(u32 instr)
{
    (void)instr;
}

void vmadd(u32 instr)
{
    mul_and_arith<add_t>(instr, vf[FS]);
}

void vmadda(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, vf[FT]);
}

void vmaddai(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, I);
}

void vmaddaq(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, Q);
}

void vmaddabc(u32 instr)
{
    mul_and_arith_acc<add_t>(instr, vf[FT][BC]);
}

void vmaddi(u32 instr)
{
    mul_and_arith<add_t>(instr, I);
}

void vmaddq(u32 instr)
{
    mul_and_arith<add_t>(instr, Q);
}

void vmaddbc(u32 instr)
{
    mul_and_arith<add_t>(instr, vf[FS][BC]);
}

void vmax(u32 instr)
{
    max(instr, vf[FT]);
}

void vmaxi(u32 instr)
{
    max(instr, I);
}

void vmaxbc(u32 instr)
{
    max(instr, vf[FT][BC]);
}

void vmfir(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            vf.set(FT, i, vi[IS]);
            mac.update(i, vi[IS]);
        }
    }
}

void vmini(u32 instr)
{
    min(instr, vf[FT]);
}

void vminii(u32 instr)
{
    min(instr, I);
}

void vminibc(u32 instr)
{
    min(instr, vf[FT][BC]);
}

void vmove(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            vf.set(FT, i, vf[FS][i]);
            mac.update(i, vf[FS][i]);
        }
    }
}

void vmr32(u32 instr)
{
    (void)instr;
}

void vmsub(u32 instr)
{
    mul_and_arith<sub_t>(instr, vf[FS]);
}

void vmsuba(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, vf[FT]);
}

void vmsubai(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, I);
}

void vmsubaq(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, Q);
}

void vmsubabc(u32 instr)
{
    mul_and_arith_acc<sub_t>(instr, vf[FT][BC]);
}

void vmsubi(u32 instr)
{
    mul_and_arith<sub_t>(instr, I);
}

void vmsubq(u32 instr)
{
    mul_and_arith<sub_t>(instr, Q);
}

void vmsubbc(u32 instr)
{
    mul_and_arith<sub_t>(instr, vf[FS][BC]);
}

void vmtir(u32 instr)
{
    vi[IT] = std::bit_cast<u32>(vf[FS][instr >> 21 & 3]);
}

void vmul(u32 instr)
{
    arith<mul_t>(instr, vf[FS]);
}

void vmula(u32 instr)
{
    arith_acc<mul_t>(instr, vf[FT]);
}

void vmulai(u32 instr)
{
    arith_acc<mul_t>(instr, I);
}

void vmulaq(u32 instr)
{
    arith_acc<mul_t>(instr, Q);
}

void vmulabc(u32 instr)
{
    arith_acc<mul_t>(instr, vf[FT][BC]);
}

void vmuli(u32 instr)
{
    arith<mul_t>(instr, I);
}

void vmulq(u32 instr)
{
    arith<mul_t>(instr, Q);
}

void vmulbc(u32 instr)
{
    arith<mul_t>(instr, vf[FS][BC]);
}

void vnop(u32 instr)
{
    (void)instr;
}

void vopmsub(u32 instr)
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

void vopmula(u32 instr)
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

void vrget(u32 instr)
{
    for (int i = 0; i < 4; ++i) {
        if (DST(i)) {
            vf.set(FT, i, R);
            mac.update(i, R);
        }
    }
}

void vrinit(u32 instr)
{
    (void)instr;
}

void vrnext(u32 instr)
{
    (void)instr;
}

void vrsqrt(u32 instr)
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

void vrxor(u32 instr)
{
    (void)instr;
    // R = R ^ vf[FS][instr >> 21 & 3];
}

void vsqd(u32 instr)
{
    (void)instr;
}

void vsqi(u32 instr)
{
    (void)instr;
}

void vsqrt(u32 instr)
{
    EeF32 op = vf[FT][instr >> 23 & 3];
    if (op >= 0.f) {
        Q = std::sqrt(op);
    } else {
        status.signal_invalid();
    }
}

void vsub(u32 instr)
{
    arith<sub_t>(instr, vf[FS]);
}

void vsuba(u32 instr)
{
    arith_acc<sub_t>(instr, vf[FT]);
}

void vsubai(u32 instr)
{
    arith_acc<sub_t>(instr, I);
}

void vsubaq(u32 instr)
{
    arith_acc<sub_t>(instr, Q);
}

void vsubabc(u32 instr)
{
    arith_acc<sub_t>(instr, vf[FT][BC]);
}

void vsubi(u32 instr)
{
    arith<sub_t>(instr, I);
}

void vsubq(u32 instr)
{
    arith<sub_t>(instr, Q);
}

void vsubbc(u32 instr)
{
    arith<sub_t>(instr, vf[FS][BC]);
}

void vwaitq(u32 instr)
{
    (void)instr;
}

} // namespace ee::vu
