#include "ee.hpp"
#include "cop0.hpp"
#include "exceptions.hpp"
#include "frontend/message.hpp"
#include "mips/disassembler.hpp"
#include "mips/mips.hpp"
#include "mmu.hpp"
#include "scheduler.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <string>
#include <utility>

namespace ee {

static u16 intc_mask, intc_stat;
static u32 cycle_counter;
static u64 time_last_step_begin;

static void check_int0();
static void fetch_decode_exec();

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

void check_int0()
{
    auto int0 = [] { return cop0.cause.ip_intc & cop0.status.im_intc; };
    bool prev_int0 = int0();
    cop0.cause.ip_intc = bool(intc_stat & intc_mask & 0x7FFF);
    if (interrupts_are_enabled() && !prev_int0 && int0()) {
        interrupt_exception();
    }
}

void fetch_decode_exec()
{
    u32 instr = virtual_read<u32, Alignment::Aligned, MemOp::InstrFetch>(pc);
    pc += 4;
    mips::disassemble_ee<mips::CpuImpl::Interpreter>(instr);
    advance_pipeline(1);
}

u64 get_ee_time()
{
    return time_last_step_begin + cycle_counter;
}

bool init()
{
    gpr = {};
    std::memset(&lo, 0, sizeof(lo));
    std::memset(&hi, 0, sizeof(hi));
    jump_addr = pc = 0;
    in_branch_delay_slot = false;
    intc_mask = intc_stat = 0;

    reset_exception();

    return true;
}

bool interrupts_are_enabled()
{
    return (cop0.status.raw & 0x10007) == 0x10001; // IE = 1, EXL = 0, ERL = 0, EIE = 1
}

void jump(u32 target)
{
    assert(!in_branch_delay_slot); // TODO: going beyond this can result in stack overflow. First need to know what the
                                   // behaviour is
    in_branch_delay_slot = true;
    fetch_decode_exec();
    in_branch_delay_slot = false;
    if (!exception_occurred) pc = target;
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

void lower_intc(Interrupt interrupt)
{
    intc_stat &= u16(~std::to_underlying(interrupt));
    cop0.cause.ip_intc = bool(intc_stat & intc_mask);
}

void raise_intc(Interrupt interrupt)
{
    intc_stat |= std::to_underlying(interrupt);
    check_int0();
}

u16 read_intc_mask()
{
    return intc_mask;
}

u16 read_intc_stat()
{
    return intc_stat;
}

u32 run(u32 cycles)
{
    cycle_counter = 0;
    while (cycle_counter < cycles) {
        fetch_decode_exec();
    }
    time_last_step_begin += cycle_counter;
    return cycle_counter;
}

void write_intc_mask(u16 data)
{
    intc_stat &= ~data;
    cop0.cause.ip_intc = bool(intc_stat & intc_mask & 0x7FFF);
}

void write_intc_stat(u16 data)
{
    intc_mask ^= data; // TODO: bit 15?
    check_int0();
}

} // namespace ee
