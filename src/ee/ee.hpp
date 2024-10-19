#pragma once

#include "mips/gpr.hpp"
#include "numtypes.hpp"
#include "reg128.hpp"

#include <filesystem>

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

inline constexpr u32 ee_clock = 294'912'000;
inline constexpr u32 bus_clock = ee_clock / 2;

inline mips::Gpr<Reg128> gpr;
inline bool in_branch_delay_slot;
inline u32 jump_addr, pc;
inline Reg128 lo, hi;
inline u32 sa;

void add_initial_events();
void advance_pipeline(u32 cycles);
u64 get_time();
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
