#pragma once

#include "mips/types.hpp"
#include "numtypes.hpp"

#include <filesystem>

namespace ee {

inline constexpr u32 ee_clock = 294'912'000;
inline constexpr u32 bus_clock = ee_clock / 2;

inline std::array<u128, 32> gpr;
inline bool in_branch_delay_slot_taken, in_branch_delay_slot_not_taken;
inline u32 jump_addr, pc;
inline u128 lo, hi;
inline u32 sa;
inline u32 cycle_counter;
inline mips::OperatingMode operating_mode;

void add_initial_events();
void advance_pipeline(u32 cycles);
u64 get_ee_time();
void init();
bool load_bios(std::filesystem::path const& path);
u32 run(u32 cycles);

} // namespace ee
