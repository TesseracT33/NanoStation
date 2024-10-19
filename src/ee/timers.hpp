#pragma once

#include "numtypes.hpp"

namespace ee::timers {

void init();
void on_hblank(bool active);
void on_vblank(bool active);
u32 read_io(u32 addr);
void write_io(u32 addr, u32 data);

} // namespace ee::timers
