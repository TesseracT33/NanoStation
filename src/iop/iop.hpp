#pragma once

#include "numtypes.hpp"

#include <array>
#include <concepts>
#include <filesystem>

namespace iop {

enum class Interrupt : u32 {
    VBlankStart = 1 << 0,
    GPU = 1 << 1,
    CDVD = 1 << 2,
    DMA = 1 << 3,
    Timer0 = 1 << 4,
    Timer1 = 1 << 5,
    Timer2 = 1 << 6,
    SIO0 = 1 << 7,
    SIO1 = 1 << 8,
    SPU2 = 1 << 9,
    PIO = 1 << 10,
    VBlankEnd = 1 << 11,
    DVD = 1 << 12,
    PCMCIA = 1 << 13,
    Timer3 = 1 << 14,
    Timer4 = 1 << 15,
    Timer5 = 1 << 16,
    SIO2 = 1 << 17,
    HTR0 = 1 << 18,
    HTR1 = 1 << 19,
    HTR2 = 1 << 20,
    HTR3 = 1 << 21,
    USB = 1 << 22,
    EXTR = 1 << 23,
    FWRE = 1 << 24,
    FDMA = 1 << 25
};

inline std::array<u32, 32> gpr;
inline bool in_branch_delay_slot;
inline u32 lo, hi, jump_addr, pc;

void add_initial_events();
void advance_pipeline(u32 cycles);
void check_int0();
u64 get_time();
bool init();
void lower_interrupt(Interrupt interrupt);
void jump(u32 target);
bool load_bios(std::filesystem::path const& path);
void raise_interrupt(Interrupt interrupt);
u32 read_i_ctrl();
u32 read_i_mask();
u32 read_i_stat();
u32 run(u32 cycles);
void write_i_ctrl(u32 value);
void write_i_mask(u32 value);
void write_i_stat(u32 value);

} // namespace iop
