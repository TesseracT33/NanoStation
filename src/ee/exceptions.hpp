#pragma once

#include "types.hpp"

namespace ee {
enum class MemOp;

inline bool exception_occurred;

void address_error_exception(u32 vaddr, MemOp mem_op);
void breakpoint_exception();
void bus_error_exception(u32 paddr, MemOp mem_op);
void coprocessor_unusable_exception(u32 cop);
void debug_exception();
void floating_point_exception();
void integer_overflow_exception();
void interrupt_exception();
void nmi_exception();
void perf_counter_exception();
void reserved_instruction_exception();
void reset_exception();
void sio_exception();
void syscall_exception();
void tlb_invalid_exception(u32 vaddr, MemOp mem_op);
void tlb_mod_exception(u32 vaddr);
void tlb_refill_exception(u32 vaddr, MemOp mem_op);
void trap_exception();
} // namespace ee
