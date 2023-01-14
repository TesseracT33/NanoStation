#include "ee_interpreter.hpp"
#include "ee.hpp"

void ee::interpreter::add(u32 rs, u32 rt, u32 rd)
{
    s32 sum = gpr.get<s32>(rs) + gpr.get<s32>(rt);
    if ((gpr.get<s32>(rs) ^ sum) & (gpr.get<s32>(rt) ^ sum) & 0x8000'0000) {

    } else {
        gpr.set(rd, sum);
    }
}

void ee::interpreter::addi(u32 rs, u32 rt, s16 imm)
{
    s32 sum = gpr.get<s32>(rs) + imm;
    if ((gpr.get<s32>(rs) ^ sum) & (imm ^ sum) & 0x8000'0000) {

    } else {
        gpr.set(rt, sum);
    }
}
