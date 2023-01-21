#pragma once

#include "types.hpp"

namespace ee {
void bc0f(s16 imm);
void bc0fl(s16 imm);
void bc0t(s16 imm);
void bc0tl(s16 imm);
void di();
void ei();
void eret();
void mfc0(u32 rd, u32 rt);
void mtc0(u32 rd, u32 rt);
void tlbp();
void tlbr();
void tlbwi();
void tlbwr();
} // namespace ee
