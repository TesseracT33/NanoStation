#include "ee.hpp"
#include "cop0.hpp"
#include "exceptions.hpp"
#include "frontend/message.hpp"
#include "jit.hpp"
#include "mips/decoder.hpp"
#include "mmu.hpp"
#include "scheduler.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <string>

namespace ee {

static u64 time_last_step_begin;

void add_initial_events()
{
    reload_count_compare_event<true>();
}

void advance_pipeline(u32 cycles)
{
    cycle_counter += cycles;
    cop0.count += cycles;
    cycles_since_updated_random += cycles;
}

u64 get_ee_time()
{
    return time_last_step_begin + cycle_counter;
}

void init()
{
    gpr = {};
    lo = {};
    hi = {};
    jump_addr = pc = 0;
    in_branch_delay_slot_taken = false;
    in_branch_delay_slot_not_taken = false;

    reset_exception();
}

bool load_bios(std::filesystem::path const& path)
{
    std::expected<std::vector<u8>, std::string> expected_bios = read_file(path, bios_size);
    if (expected_bios) {
        std::vector<u8> const& bios_val = expected_bios.value();
        std::copy(bios_val.cbegin(), bios_val.cend(), bios.begin());
        return true;
    } else {
        message::error(std::string("Failed to load bios; ") + expected_bios.error());
        return false;
    }
}

u32 run(u32 cycles)
{
    return RunJit(cycles);
}

} // namespace ee
