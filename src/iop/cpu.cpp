#include "cpu.hpp"
#include "exceptions.hpp"
#include "iop.hpp"
#include "memory.hpp"

#include <algorithm>
#include <limits>

namespace iop {

static u64 muldiv_delay;
static u64 time_last_muldiv_start;

static void add_muldiv_delay(u64 cycles)
{
    u64 time = get_time();
    if (time < time_last_muldiv_start + muldiv_delay) { // current mult/div hasn't finished yet
        u64 delta = time_last_muldiv_start + muldiv_delay - time;
        advance_pipeline(delta); // block until current mult/div finishes
        time_last_muldiv_start = time + delta;
    } else {
        time_last_muldiv_start = time;
    }
    muldiv_delay = cycles;
}

static void block_lohi_read()
{
    u64 time = get_time();
    if (time < time_last_muldiv_start + muldiv_delay) { // current mult/div hasn't finished yet
        advance_pipeline(time_last_muldiv_start + muldiv_delay - time); // block until current mult/div finishes
    }
}

void add(u32, u32, u32)
{
}

void addi(u32, u32, s16)
{
}

void addiu(u32, u32, s16)
{
}

void addu(u32, u32, u32)
{
}

void and_(u32, u32, u32)
{
}

void andi(u32, u32, u16)
{
}

void beq(u32, u32, s16)
{
}

void bgez(u32, s16)
{
}

void bgezal(u32, s16)
{
}

void bgtz(u32, s16)
{
}

void blez(u32, s16)
{
}

void bltz(u32, s16)
{
}

void bltzal(u32, s16)
{
}

void bne(u32, u32, s16)
{
}

void break_()
{
}

void div(u32, u32)
{
}

void divu(u32, u32)
{
}

void j(u32)
{
}

void jal(u32)
{
}

void jalr(u32, u32)
{
}

void jr(u32)
{
}

void lb(u32, u32, s16)
{
}

void lbu(u32, u32, s16)
{
}

void lh(u32, u32, s16)
{
}

void lhu(u32, u32, s16)
{
}

void lui(u32, s16)
{
}

void lw(u32, u32, s16)
{
}

void lwl(u32, u32, s16)
{
}

void lwr(u32, u32, s16)
{
}

void lwu(u32, u32, s16)
{
}

void mfhi(u32)
{
}

void mflo(u32)
{
}

void mthi(u32)
{
}

void mtlo(u32)
{
}

void mult(u32, u32)
{
}

void multu(u32, u32)
{
}

void nor(u32, u32, u32)
{
}

void or_(u32, u32, u32)
{
}

void ori(u32, u32, u16)
{
}

void sb(u32, u32, s16)
{
}

void sh(u32, u32, s16)
{
}

void sll(u32, u32, u32)
{
}

void sllv(u32, u32, u32)
{
}

void slt(u32, u32, u32)
{
}

void slti(u32, u32, s16)
{
}

void sltiu(u32, u32, s16)
{
}

void sltu(u32, u32, u32)
{
}

void sra(u32, u32, u32)
{
}

void srav(u32, u32, u32)
{
}

void srl(u32, u32, u32)
{
}

void srlv(u32, u32, u32)
{
}

void sub(u32, u32, u32)
{
}

void subu(u32, u32, u32)
{
}

void sw(u32, u32, s16)
{
}

void swl(u32, u32, s16)
{
}

void swr(u32, u32, s16)
{
}

void syscall()
{
}

void xor_(u32, u32, u32)
{
}

void xori(u32, u32, u16)
{
}

} // namespace iop
