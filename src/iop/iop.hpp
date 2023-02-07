#pragma once

#include "types.hpp"

#include <array>
#include <concepts>
#include <filesystem>

namespace iop {

struct GPR {
    u32 get(u32 idx) const { return gpr[idx]; }
    u32 operator[](u32 idx) const { return gpr[idx]; } // return by value so that writes have to be made through 'set'
    void set(u32 idx, std::integral auto val)
        requires(sizeof(val) <= 4)
    {
        gpr[idx] = val;
        gpr[0] = 0;
    }

private:
    std::array<u32, 32> gpr{};
} inline gpr;

inline bool in_branch_delay_slot;
inline u32 lo, hi, jump_addr, pc;

void add_initial_events();
void advance_pipeline(u32 cycles);
void check_interrupts();
bool init();
void jump(u32 target);
bool load_bios(std::filesystem::path const& path);
u32 run(u32 cycles);

} // namespace iop
