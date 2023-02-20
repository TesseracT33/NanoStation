#pragma once

#include "mips/gpr.hpp"
#include "types.hpp"

#include <array>
#include <concepts>
#include <filesystem>

namespace iop {

inline mips::Gpr<u32> gpr;
inline bool in_branch_delay_slot;
inline u32 lo, hi, jump_addr, pc;

void add_initial_events();
void advance_pipeline(u32 cycles);
void check_interrupts();
u64 get_global_time();
bool init();
void jump(u32 target);
bool load_bios(std::filesystem::path const& path);
u32 run(u32 cycles);

} // namespace iop
