#include "intc.hpp"
#include "cop0.hpp"
#include "exceptions.hpp"

#include <utility>

namespace ee {

static u16 intc_mask, intc_stat;

static void update_int0();
static void update_int0_and_check_interrupts();

void init_intc()
{
    intc_mask = intc_stat = 0;
    update_int0();
}

bool interrupts_are_enabled()
{
    return (cop0.status.raw & 0x10007) == 0x10001; // IE = 1, EXL = 0, ERL = 0, EIE = 1
}

void lower_intc(Interrupt interrupt)
{
    intc_stat &= u16(~std::to_underlying(interrupt));
    update_int0();
}

void raise_intc(Interrupt interrupt)
{
    intc_stat |= std::to_underlying(interrupt);
    update_int0_and_check_interrupts();
}

u16 read_intc_mask()
{
    return intc_mask;
}

u16 read_intc_stat()
{
    return intc_stat;
}

void update_int0()
{
    cop0.cause.ip_intc = (intc_stat & intc_mask & 0x7FFF) != 0;
}

void update_int0_and_check_interrupts()
{
    auto int0 = [] { return (cop0.cause.ip_intc & cop0.status.im_intc) != 0; };
    bool prev_int0 = int0();
    update_int0();
    if (interrupts_are_enabled() && !prev_int0 && int0()) {
        interrupt_exception();
    }
}

void write_intc_mask(u16 data)
{
    intc_stat ^= data; // (0=No effect, 1=Reverse)
    update_int0_and_check_interrupts();
}

void write_intc_stat(u16 data)
{
    intc_mask &= ~data; // (0=No effect, 1=Clear)
    update_int0();
}

} // namespace ee
