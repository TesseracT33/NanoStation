#include "cpu.hpp"
#include "exceptions.hpp"
#include "iop.hpp"
#include "memory.hpp"

#include <algorithm>
#include <limits>

namespace iop {

using enum mips::CpuImpl;

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

template<> void add<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    s32 sum;
    bool overflow;
#if HAS_BUILTIN_ADD_OVERFLOW
    overflow = __builtin_add_overflow(gpr[rs], gpr[rt], &sum);
#else
    sum = gpr[rs] + gpr[rt];
    overflow = (gpr[rs] ^ sum) & (gpr[rt] ^ sum) & 0x8000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rd, sum);
    }
}

template<> void addi<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 sum;
    bool overflow;
#if HAS_BUILTIN_ADD_OVERFLOW
    overflow = __builtin_add_overflow(gpr[rs], imm, &sum);
#else
    sum = gpr[rs] + imm;
    overflow = (gpr[rs] ^ sum) & (imm ^ sum) & 0x8000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rt, sum);
    }
}

template<> void addiu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs] + imm);
}

template<> void addu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs] + gpr[rt]);
}

template<> void and_<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs] & gpr[rt]);
}

template<> void andi<Interpreter>(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs] & imm);
}

template<> void beq<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs] == gpr[rt]) {
        jump(pc + (imm << 2));
    }
}

template<> void bgez<Interpreter>(u32 rs, s16 imm)
{
    if (s32(gpr[rs]) >= 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bgezal<Interpreter>(u32 rs, s16 imm)
{
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
    if (s32(gpr[rs]) >= 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bgtz<Interpreter>(u32 rs, s16 imm)
{
    if (s32(gpr[rs]) > 0) {
        jump(pc + (imm << 2));
    }
}

template<> void blez<Interpreter>(u32 rs, s16 imm)
{
    if (s32(gpr[rs]) <= 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bltz<Interpreter>(u32 rs, s16 imm)
{
    if (s32(gpr[rs]) < 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bltzal<Interpreter>(u32 rs, s16 imm)
{
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
    if (s32(gpr[rs]) < 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bne<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs] != gpr[rt]) {
        jump(pc + (imm << 2));
    }
}

template<> void break_<Interpreter>()
{
    breakpoint_exception();
}

template<> void div<Interpreter>(u32 rs, u32 rt)
{
    s32 op1 = gpr[rs];
    s32 op2 = gpr[rt];
    if (op2 == 0) {
        lo = op1 >= 0 ? -1 : 1;
        hi = op1;
    } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
        lo = std::numeric_limits<s32>::min();
        hi = 0;
    } else [[likely]] {
        lo = op1 / op2;
        hi = op1 % op2;
    }
    add_muldiv_delay(36);
}

template<> void divu<Interpreter>(u32 rs, u32 rt)
{
    u32 op1 = gpr[rs];
    u32 op2 = gpr[rt];
    if (op2 == 0) {
        lo = -1;
        hi = s32(op1);
    } else {
        lo = s32(op1 / op2);
        hi = s32(op1 % op2);
    }
    add_muldiv_delay(36);
}

template<> void j<Interpreter>(u32 imm26)
{
    if (!in_branch_delay_slot) {
        jump(pc & 0xF000'0000 | imm26 << 2);
    }
}

template<> void jal<Interpreter>(u32 imm26)
{
    if (!in_branch_delay_slot) {
        jump(pc & 0xF000'0000 | imm26 << 2);
    }
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
}

template<> void jalr<Interpreter>(u32 rs, u32 rd)
{
    if (!in_branch_delay_slot) {
        u32 target = gpr[rs];
        if (target & 3) {
            address_error_exception(target, MemOp::InstrFetch);
        } else {
            jump(target);
        }
    }
    gpr.set(rd, 4 + (in_branch_delay_slot ? jump_addr : pc));
}

template<> void jr<Interpreter>(u32 rs)
{
    if (!in_branch_delay_slot) {
        u32 target = gpr[rs];
        if (target & 3) {
            address_error_exception(target, MemOp::InstrFetch);
        } else {
            jump(target);
        }
    }
}

template<> void lb<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s8 val = read<u8>(gpr[rs] + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lbu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u8 val = read<u8>(gpr[rs] + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lh<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s16 val = read<u16>(gpr[rs] + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lhu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u16 val = read<u16>(gpr[rs] + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lui<Interpreter>(u32 rt, s16 imm)
{
    gpr.set(rt, imm << 16);
}

template<> void lw<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 val = read<u32>(gpr[rs] + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lwl<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs] + imm;
    s32 val = read<u32, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 load_mask = s32(0xFF00'0000) >> (8 * (addr & 3));
        gpr.set(rt, val & load_mask | gpr[rt] & ~load_mask);
    }
}

template<> void lwr<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs] + imm;
    s32 val = read<u32, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 bits_offset = 8 * (addr & 3);
        u32 load_mask = 0xFFFF'FFFF >> bits_offset;
        val >>= bits_offset;
        gpr.set(rt, val & load_mask | gpr[rt] & ~load_mask);
    }
}

template<> void lwu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u32 val = read<u32>(gpr[rs] + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void mfhi<Interpreter>(u32 rd)
{
    block_lohi_read();
    gpr.set(rd, hi);
}

template<> void mflo<Interpreter>(u32 rd)
{
    block_lohi_read();
    gpr.set(rd, lo);
}

template<> void mthi<Interpreter>(u32 rs)
{ // TODO: what happens if writing to lo/hi during mult/div?
    hi = gpr[rs];
}

template<> void mtlo<Interpreter>(u32 rs)
{
    lo = gpr[rs];
}

template<> void mult<Interpreter>(u32 rs, u32 rt)
{
    s64 op1 = u32(gpr[rs]), op2 = u32(gpr[rt]);
    s64 prod = op1 * op2;
    lo = u32(prod);
    hi = u32(prod >> 32);
    //  Fast  (6 cycles)   rs = 00000000h..000007FFh, or rs = FFFFF800h..FFFFFFFFh
    //  Med   (9 cycles)   rs = 00000800h..000FFFFFh, or rs = FFF00000h..FFFFF801h
    //  Slow  (13 cycles)  rs = 00100000h..7FFFFFFFh, or rs = 80000000h..FFF00001h
    // https://problemkaputt.de/psx-spx.htm#cpualuopcodes
    u32 cycles = [op1_abs_s32 = std::abs(s32(op1))] { // abs(int32_min) == int32_min
        if (u32(op1_abs_s32) < 0x800) return 6;
        if (u32(op1_abs_s32) < 0x10'0000) return 9;
        return 13;
    }();
    add_muldiv_delay(cycles);
}

template<> void multu<Interpreter>(u32 rs, u32 rt)
{
    u64 op1 = u32(gpr[rs]), op2 = u32(gpr[rt]);
    u64 prod = op1 * op2;
    lo = u32(prod);
    hi = u32(prod >> 32);
    //  Fast  (6 cycles)   rs = 00000000h..000007FFh
    //  Med   (9 cycles)   rs = 00000800h..000FFFFFh
    //  Slow  (13 cycles)  rs = 00100000h..FFFFFFFFh
    u32 cycles = [op1_u32 = u32(op1)] {
        if (op1_u32 < 0x800) return 6;
        if (op1_u32 < 0x10'0000) return 9;
        return 13;
    }();
    add_muldiv_delay(cycles);
}

template<> void nor<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, ~(gpr[rs] | gpr[rt]));
}

template<> void or_<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs] | gpr[rt]);
}

template<> void ori<Interpreter>(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs] | imm);
}

template<> void sb<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    write<1>(gpr[rs] + imm, u8(gpr[rt]));
}

template<> void sh<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    write<2>(gpr[rs] + imm, u16(gpr[rt]));
}

template<> void sll<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt] << sa);
}

template<> void sllv<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt] << (gpr[rs] & 31));
}

template<> void slt<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, s32(gpr[rs]) < s32(gpr[rt]));
}

template<> void slti<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, s32(gpr[rs]) < imm);
}

template<> void sltiu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, u32(gpr[rs]) < u32(imm));
}

template<> void sltu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, u32(gpr[rs]) < u32(gpr[rt]));
}

template<> void sra<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, s32(gpr[rt]) >> sa);
}

template<> void srav<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, s32(gpr[rt]) >> (gpr[rs] & 31));
}

template<> void srl<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, u32(gpr[rt]) >> sa);
}

template<> void srlv<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, u32(gpr[rt]) >> (gpr[rs] & 31));
}

template<> void sw<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    write<4>(gpr[rs] + imm, gpr[rt]);
}

template<> void swl<Interpreter>(u32 rs, u32 rt, s16 imm)
{
}

template<> void swr<Interpreter>(u32 rs, u32 rt, s16 imm)
{
}

template<> void syscall<Interpreter>()
{
    syscall_exception();
}

template<> void sub<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    s32 sum;
    bool overflow;
#if HAS_BUILTIN_SUB_OVERFLOW
    overflow = __builtin_sub_overflow(gpr[rs], gpr[rt], &sum);
#else
    sum = gpr[rs] - gpr[rt];
    overflow = (gpr[rs] ^ gpr[rt]) & ~(gpr[rt] ^ sum) & 0x8000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rd, sum);
    }
}

template<> void subu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs] - gpr[rt]);
}

template<> void xor_<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs] ^ gpr[rt]);
}

template<> void xori<Interpreter>(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs] ^ imm);
}

} // namespace iop
