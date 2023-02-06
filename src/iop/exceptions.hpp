#pragma once

#include "types.hpp"

namespace iop {
enum class MemOp;

extern bool exception_occurred;

void address_error_exception(u32 vaddr, MemOp mem_op);
void breakpoint_exception();
void bus_error_exception(u32 paddr, MemOp mem_op);
void coprocessor_unusable_exception(u32 cop);
void integer_overflow_exception();
void interrupt_exception();
void nmi_exception();
void reserved_instruction_exception();
void reset_exception();
void syscall_exception();

} // namespace iop
