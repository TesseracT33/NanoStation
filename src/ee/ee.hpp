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

inline mips::Gpr<Reg128> gpr;
inline bool in_branch_delay_slot;
inline u32 jump_addr, pc;
inline Reg128 lo, hi;
inline u32 sa;

void add_initial_events();
void advance_pipeline(u32 cycles);
void check_interrupts();
bool init();
void jump(u32 target);
bool load_bios(std::filesystem::path const& path);
u32 run(u32 cycles);

} // namespace ee
