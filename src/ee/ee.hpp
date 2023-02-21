#pragma once

#include "cpu.hpp"
#include "mips/gpr.hpp"
#include "reg128.hpp"
#include "types.hpp"

#include <array>
#include <concepts>
#include <cstring>
#include <filesystem>
#include <utility>

namespace ee {

enum class Interrupt : u16 {
    GS,
    SBUS,
    VBlankStart,
    VBlankEnd,
    VIF0,
    VIF1,
    VU0,
    VU1,
    IPU,
    Timer0,
    Timer1,
    Timer2,
    Timer3,
    SFIFO,
    VU0Watchdog
};

inline mips::Gpr<Reg128> gpr;
inline bool in_branch_delay_slot;
inline u32 jump_addr, pc;
inline Reg128 lo, hi;
inline u32 sa;

void add_initial_events();
void advance_pipeline(u32 cycles);
bool init();
bool interrupts_are_enabled();
void jump(u32 target);
bool load_bios(std::filesystem::path const& path);
void lower_intc(Interrupt interrupt);
void raise_intc(Interrupt interrupt);
u16 read_intc_mask();
u16 read_intc_stat();
u32 run(u32 cycles);
void write_intc_mask(u16 data);
void write_intc_stat(u16 data);

} // namespace ee
