#pragma once

#include "numtypes.hpp"

namespace ee {

enum class Interrupt : u16 {
    GS = 1 << 0,
    SBUS = 1 << 1,
    VBlankStart = 1 << 2,
    VBlankEnd = 1 << 3,
    VIF0 = 1 << 4,
    VIF1 = 1 << 5,
    VU0 = 1 << 6,
    VU1 = 1 << 7,
    IPU = 1 << 8,
    Timer0 = 1 << 9,
    Timer1 = 1 << 10,
    Timer2 = 1 << 11,
    Timer3 = 1 << 12,
    SFIFO = 1 << 13,
    VU0Watchdog = 1 << 14
};

void init_intc();
bool interrupts_are_enabled();
void lower_intc(Interrupt interrupt);
void raise_intc(Interrupt interrupt);
u16 read_intc_mask();
u16 read_intc_stat();
void write_intc_mask(u16 data);
void write_intc_stat(u16 data);

} // namespace ee
