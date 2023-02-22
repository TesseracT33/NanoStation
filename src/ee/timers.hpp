#pragma once

#include "types.hpp"

namespace ee::timers {

void init();
void on_hblank(bool active);
void on_vblank(bool active);
u32 read(u32 addr);
void write(u32 addr, u32 data);

} // namespace ee::timers
