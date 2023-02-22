#pragma once

#include "mips/gpr.hpp"
#include "types.hpp"

#include <array>
#include <concepts>
#include <filesystem>

namespace iop {

enum class Interrupt : u32 {
    VBlankStart,
    GPU,
    CDVD,
    DMA,
    Timer0,
    Timer1,
    Timer2,
    SIO0,
    SIO1,
    SPU2,
    PIO,
    VBlankEnd,
    DVD,
    PCMCIA,
    Timer3,
    Timer4,
    Timer5,
    SIO2,
    HTR0,
    HTR1,
    HTR2,
    HTR3,
    USB,
    EXTR,
    FWRE,
    FDMA
};

inline mips::Gpr<u32> gpr;
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
