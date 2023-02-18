#include "exceptions.hpp"
#include "cop0.hpp"
#include "ee.hpp"
#include "mmu.hpp"

namespace ee {

static u32 get_common_vector();
static void handle_lvl1_exception();
static void handle_lvl2_exception();
static void on_tlb_exception(u32 vaddr);

void address_error_exception(u32 vaddr, MemOp mem_op)
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = mem_op == MemOp::DataWrite ? 5 : 4;
    cop0.bad_v_addr = vaddr;
}

void breakpoint_exception()
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 9;
}

void bus_error_exception(u32 paddr, MemOp mem_op)
{
    // TODO: maskable
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = mem_op == MemOp::InstrFetch ? 6 : 7;
    if (!cop0.status.bem) cop0.bad_p_addr.value = paddr;
}

void coprocessor_unusable_exception(u32 cop)
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 11;
    cop0.cause.ce = cop;
}

void debug_exception()
{
    handle_lvl2_exception();
    pc = cop0.status.dev ? 0xBFC0'0300 : 0x8000'0100;
    cop0.cause.exc2 = 3;
}

void floating_point_exception()
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 15;
}

u32 get_common_vector()
{
    return cop0.status.bev ? 0xBFC0'0380 : 0x8000'0180;
}

void handle_lvl1_exception()
{
    if (!cop0.status.exl) {
        cop0.status.exl = 1;
        cop0.cause.bd = in_branch_delay_slot;
        cop0.epc = pc - (in_branch_delay_slot ? 8 : 4);
    }
}

void handle_lvl2_exception()
{
    cop0.status.erl = 1;
    cop0.cause.bd2 = in_branch_delay_slot;
    cop0.error_epc = pc - (in_branch_delay_slot ? 8 : 4);
}

void integer_overflow_exception()
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 12;
}

void interrupt_exception()
{
    handle_lvl1_exception();
    pc = cop0.status.bev ? 0xBFC0'0400 : 0x8000'0200;
    cop0.cause.exc_code = 0;
}

void nmi_exception()
{
    handle_lvl2_exception();
    pc = 0xBFC0'0000;
    cop0.cause.exc2 = 1;
    cop0.status.bev = 1;
}

void on_tlb_exception(u32 vaddr)
{
    cop0.bad_v_addr = vaddr;
    cop0.context.bad_vpn2 = vaddr >> 13;
    // TODO: set entry_hi.pfn
}

void perf_counter_exception()
{
    handle_lvl2_exception();
    pc = cop0.status.dev ? 0xBFC0'0280 : 0x8000'0080;
    cop0.cause.exc2 = 2;
}

void reserved_instruction_exception()
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 10;
}

void reset_exception()
{
    handle_lvl2_exception();
    pc = 0xBFC0'0000;
    cop0.cause.exc2 = 0;
    cop0.status.bev = 1;
    cop0.status.bem = 0;
    cop0.config.bpe = cop0.config.dce = cop0.config.die = cop0.config.ice = cop0.config.nbe = 0;
    cop0.random = 47;
    cop0.wired = 0;
    cop0.perf_pccr.cte = 0;
    cop0.debug.dre = cop0.debug.dwe = cop0.debug.iae = 0; // TODO: it's only breakpoint control register
    // TODO: Valid, Dirty, LRF, and Lock bits of the data cache and the Valid and LRF bits of the instruction cache are
    // initialized to 0 on reset.
}

void sio_exception()
{
    // TODO: maskable
    handle_lvl2_exception();
    pc = cop0.status.dev ? 0xBFC0'0300 : 0x8000'0100;
    cop0.cause.exc2 = 3;
    cop0.cause.siop = 1;
}

void syscall_exception()
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 8;
}

void tlb_invalid_exception(u32 vaddr, MemOp mem_op)
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = mem_op == MemOp::DataWrite ? 3 : 2;
    on_tlb_exception(vaddr);
}

void tlb_mod_exception(u32 vaddr)
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 1;
    on_tlb_exception(vaddr);
}

void tlb_refill_exception(u32 vaddr, MemOp mem_op)
{
    u32 prev_exl = cop0.status.exl;
    handle_lvl1_exception();
    pc = cop0.status.bev ? 0xBFC0'0200 : 0x8000'0000;
    if (prev_exl) pc |= 0x180;
    cop0.cause.exc_code = mem_op == MemOp::DataWrite ? 3 : 2;
    on_tlb_exception(vaddr);
}

void trap_exception()
{
    handle_lvl1_exception();
    pc = get_common_vector();
    cop0.cause.exc_code = 13;
}

} // namespace ee
