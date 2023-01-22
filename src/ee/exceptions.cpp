#include "exceptions.hpp"

namespace ee {
bool exception_occurred;

void address_error_exception(u32 addr, MemOp mem_op)
{
}

void breakpoint_exception()
{
}

void bus_error_exception(u32 addr, MemOp mem_op)
{
}

void coprocessor_unusable_exception(u32 cop)
{
}

void integer_overflow_exception()
{
}

void interrupt_exception()
{
}

void reserved_instruction_exception()
{
}

void syscall_exception()
{
}

void tlb_mod_exception()
{
}

void trap_exception()
{
}

} // namespace ee
