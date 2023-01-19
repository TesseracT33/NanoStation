#pragma once

#include "mmu.hpp"
#include "types.hpp"

namespace ee {
extern bool exception_occurred;

void address_error_exception(u32 addr, MemOp mem_op);
void integer_overflow_exception();
void reserved_instruction_exception();
void trap_exception();
}