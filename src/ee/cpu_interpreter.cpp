#include "cpu_interpreter.hpp"
#include "ee.hpp"
#include "exceptions.hpp"
#include "mmu.hpp"

#include <limits>

#ifdef __has_builtin
#if __has_builtin(__builtin_add_overflow)
#define USE_BUILTIN_ADD_OVERFLOW 1
#endif
#if __has_builtin(__builtin_sub_overflow)
#define USE_BUILTIN_SUB_OVERFLOW 1
#endif
#endif

namespace ee::interpreter {
void add(u32 rs, u32 rt, u32 rd)
{
    s32 sum;
    bool overflow;
#if USE_BUILTIN_ADD_OVERFLOW
    overflow = __builtin_add_overflow(gpr[rs].s32(), gpr[rt].s32(), &sum);
#else
    sum = gpr[rs].s32() + gpr[rt].s32();
    overflow = (gpr[rs].s32() ^ sum) & (gpr[rt].s32() ^ sum) & 0x8000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rd, sum);
    }
}

void addi(u32 rs, u32 rt, s16 imm)
{
    s32 sum;
    bool overflow;
#if USE_BUILTIN_ADD_OVERFLOW
    overflow = __builtin_add_overflow(gpr[rs].s32(), imm, &sum);
#else
    sum = gpr[rs].s32() + imm;
    overflow = (gpr[rs].s32() ^ sum) & (imm ^ sum) & 0x8000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rt, sum);
    }
}

void addiu(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs].s32() + imm);
}

void addu(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s32() + gpr[rt].s32());
}

void and_(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() & gpr[rt].s64());
}

void andi(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs].s64() & imm);
}

void beq(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() == gpr[rt].s64()) {
        jump(pc + (imm << 2));
    }
}

void beql(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() == gpr[rt].s64()) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void bgez(u32 rs, s16 imm)
{
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    }
}

void bgezal(u32 rs, s16 imm)
{
    gpr[31] = 4 + (in_branch_delay_slot ? jump_addr : pc);
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    }
}

void bgezall(u32 rs, s16 imm)
{
    gpr[31] = 4 + (in_branch_delay_slot ? jump_addr : pc);
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void bgezl(u32 rs, s16 imm)
{
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void bgtz(u32 rs, s16 imm)
{
    if (gpr[rs].s64() > 0) {
        jump(pc + (imm << 2));
    }
}

void bgtzl(u32 rs, s16 imm)
{
    if (gpr[rs].s64() > 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void blez(u32 rs, s16 imm)
{
    if (gpr[rs].s64() <= 0) {
        jump(pc + (imm << 2));
    }
}

void blezl(u32 rs, s16 imm)
{
    if (gpr[rs].s64() <= 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void bltz(u32 rs, s16 imm)
{
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    }
}

void bltzal(u32 rs, s16 imm)
{
    gpr[31] = 4 + (in_branch_delay_slot ? jump_addr : pc);
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    }
}

void bltzall(u32 rs, s16 imm)
{
    gpr[31] = 4 + (in_branch_delay_slot ? jump_addr : pc);
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void bltzl(u32 rs, s16 imm)
{
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void bne(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() != gpr[rt].s64()) {
        jump(pc + (imm << 2));
    }
}

void bnel(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() != gpr[rt].s64()) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

void break_()
{
    breakpoint_exception();
}

void cache()
{
    // TODO
}

void dadd(u32 rs, u32 rt, u32 rd)
{
    s64 sum;
    bool overflow;
#if USE_BUILTIN_ADD_OVERFLOW
    overflow = __builtin_add_overflow(gpr[rs].s64(), gpr[rt].s64(), &sum);
#else
    sum = gpr[rs].s64() + gpr[rt].s64();
    overflow = (gpr[rs].s64() ^ sum) & (gpr[rt].s64() ^ sum) & 0x8000'0000'0000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rd, sum);
    }
}

void daddi(u32 rs, u32 rt, s16 imm)
{
    s64 sum;
    bool overflow;
#if USE_BUILTIN_ADD_OVERFLOW
    overflow = __builtin_add_overflow(gpr[rs].s64(), imm, &sum);
#else
    sum = gpr[rs].s64() + imm;
    overflow = (gpr[rs].s64() ^ sum) & (imm ^ sum) & 0x8000'0000'0000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rt, sum);
    }
}

void daddiu(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs].s64() + imm);
}

void daddu(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() + gpr[rt].s64());
}

void div(u32 rs, u32 rt)
{
    s32 op1 = gpr[rs].s32();
    s32 op2 = gpr[rt].s32();
    if (op2 == 0) {
        lo = op1 >= 0 ? -1 : 1;
        hi = op1;
    } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
        lo = op1;
        hi = 0;
    } else [[likely]] {
        lo = op1 / op2;
        hi = op1 % op2;
    }
}

void div1(u32 rs, u32 rt)
{
    s32 op1 = gpr[rs].s32();
    s32 op2 = gpr[rt].s32();
    if (op2 == 0) {
        lo.set_upper_dword(op1 >= 0 ? -1 : 1);
        hi.set_upper_dword(op1);
    } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
        lo.set_upper_dword(op1);
        hi.set_upper_dword(0);
    } else [[likely]] {
        lo.set_upper_dword(op1 / op2);
        hi.set_upper_dword(op1 % op2);
    }
}

void divu(u32 rs, u32 rt)
{
    u32 op1 = gpr[rs].u32();
    u32 op2 = gpr[rt].u32();
    if (op2 == 0) {
        lo = -1;
        hi = s32(op1);
    } else {
        lo = s32(op1 / op2);
        hi = s32(op1 % op2);
    }
}

void divu1(u32 rs, u32 rt)
{
    u32 op1 = gpr[rs].u32();
    u32 op2 = gpr[rt].u32();
    if (op2 == 0) {
        lo.set_upper_dword(-1);
        hi.set_upper_dword(s32(op1));
    } else {
        lo.set_upper_dword(s32(op1 / op2));
        hi.set_upper_dword(s32(op1 % op2));
    }
}

void dsll(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() << sa);
}

void dsll32(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() << (sa + 32));
}

void dsllv(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s64() << (gpr[rs].s32() & 63));
}

void dsra(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() >> sa);
}

void dsra32(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() >> (sa + 32));
}

void dsrav(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s64() >> (gpr[rs].s32() & 63));
}

void dsrl(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].u64() >> sa);
}

void dsrl32(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].u64() >> (sa + 32));
}

void dsrlv(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].u64() >> (gpr[rs].s32() & 63));
}

void dsub(u32 rs, u32 rt, u32 rd)
{
    s64 sum;
    bool overflow;
#if USE_BUILTIN_SUB_OVERFLOW
    overflow = __builtin_sub_overflow(gpr[rs].s64(), gpr[rt].s64(), &sum);
#else
    sum = gpr[rs].s64() - gpr[rt].s64();
    overflow = (gpr[rs].s64() ^ gpr[rt].s64()) & ~(gpr[rt].s64() ^ sum) & 0x8000'0000'0000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rd, sum);
    }
}

void dsubu(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() - gpr[rt].s64());
}

void j(u32 imm26)
{
    if (!in_branch_delay_slot) {
        jump(pc & 0xF000'0000 | imm26 << 2);
    }
}

void jal(u32 imm26)
{
    if (!in_branch_delay_slot) {
        jump(pc & 0xF000'0000 | imm26 << 2);
    }
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
}

void jalr(u32 rs, u32 rd)
{
    if (!in_branch_delay_slot) {
        s32 target = gpr[rs].s32();
        if (target & 3) {
            address_error_exception(target, MemOp::InstrFetch);
        } else {
            jump(target);
        }
    }
    gpr.set(rd, 4 + (in_branch_delay_slot ? jump_addr : pc));
}

void jr(u32 rs)
{
    if (!in_branch_delay_slot) {
        s32 target = gpr[rs].s32();
        if (target & 3) {
            address_error_exception(target, MemOp::InstrFetch);
        } else {
            jump(target);
        }
    }
}

void lb(u32 rs, u32 rt, s16 imm)
{
    s8 val = virtual_read<1>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void lbu(u32 rs, u32 rt, s16 imm)
{
    u8 val = virtual_read<1>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void ld(u32 rs, u32 rt, s16 imm)
{
    s64 val = virtual_read<8>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void ldl(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s64 val = virtual_read<8, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s64 load_mask = s64(0xFF00'0000'0000'0000) >> (8 * (addr & 7));
        gpr.set(rt, val & load_mask | gpr[rt].s64() & ~load_mask);
    }
}

void ldr(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s64 val = virtual_read<8, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 bits_offset = 8 * (addr & 7);
        u64 load_mask = 0xFFFF'FFFF'FFFF'FFFF >> bits_offset;
        val >>= bits_offset;
        gpr.set(rt, val & load_mask | gpr[rt].s64() & ~load_mask);
    }
}

void lh(u32 rs, u32 rt, s16 imm)
{
    s16 val = virtual_read<2>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void lhu(u32 rs, u32 rt, s16 imm)
{
    u16 val = virtual_read<2>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void lq(u32 rs, u32 rt, s16 imm)
{
    u64x2 val = virtual_read<16>((gpr[rs].s32() + imm) & ~15);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void lui(u32 rt, s16 imm)
{
    gpr.set(rt, imm << 16);
}

void lw(u32 rs, u32 rt, s16 imm)
{
    s32 val = virtual_read<4>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void lwl(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s32 val = virtual_read<4, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 load_mask = s32(0xFF00'0000) >> (8 * (addr & 3));
        gpr.set(rt, val & load_mask | gpr[rt].s32() & ~load_mask);
    }
}

void lwr(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s32 val = virtual_read<4, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 bits_offset = 8 * (addr & 3);
        u32 load_mask = 0xFFFF'FFFF >> bits_offset;
        val >>= bits_offset;
        gpr.set(rt, val & load_mask | gpr[rt].s32() & ~load_mask);
    }
}

void lwu(u32 rs, u32 rt, s16 imm)
{
    u32 val = virtual_read<4>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

void mfhi(u32 rd)
{
    gpr.set(rd, hi);
}

void mfhi1(u32 rd)
{
    gpr.set(rd, hi.get_upper_dword());
}

void mflo(u32 rd)
{
    gpr.set(rd, lo);
}

void mflo1(u32 rd)
{
    gpr.set(rd, lo.get_upper_dword());
}

void mfsa(u32 rd)
{
    gpr.set(rd, sa);
}

void movn(u32 rs, u32 rt, u32 rd)
{
    if (gpr[rt].s64() != 0) {
        gpr.set(rd, gpr[rs].s64());
    }
}

void movz(u32 rs, u32 rt, u32 rd)
{
    if (gpr[rt].s64() == 0) {
        gpr.set(rd, gpr[rs].s64());
    }
}

void mthi(u32 rs)
{
    hi = gpr[rs].s64();
}

void mthi1(u32 rs)
{
    hi.set_upper_dword(gpr[rs].s64());
}

void mtlo(u32 rs)
{
    lo = gpr[rs].s64();
}

void mtlo1(u32 rs)
{
    lo.set_upper_dword(gpr[rs].s64());
}

void mtsa(u32 rs)
{
    sa = gpr[rs].u32();
}

void mtsab(u32 rs, s16 imm)
{
    sa = 8 * (15 & (gpr[rs].s16() ^ imm));
}

void mtsah(u32 rs, s16 imm)
{
    sa = 16 * (7 & (gpr[rs].s16() ^ imm));
}

void mult(u32 rs, u32 rt)
{
    s64 prod = s64(gpr[rs].s32()) * s64(gpr[rt].s32());
    lo = s32(prod);
    hi = prod >> 32;
}

void mult1(u32 rs, u32 rt)
{
    s64 prod = s64(gpr[rs].s32()) * s64(gpr[rt].s32());
    lo.set_upper_dword(s32(prod));
    hi.set_upper_dword(prod >> 32);
}

void multu(u32 rs, u32 rt)
{
    u64 prod = u64(gpr[rs].u32()) * u64(gpr[rt].u32());
    lo = s32(prod);
    hi = s32(prod >> 32);
}

void multu1(u32 rs, u32 rt)
{
    u64 prod = u64(gpr[rs].u32()) * u64(gpr[rt].u32());
    lo.set_upper_dword(s32(prod));
    hi.set_upper_dword(s32(prod >> 32));
}

void nor(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, ~(gpr[rs].s64() | gpr[rt].s64()));
}

void or_(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() | gpr[rt].s64());
}

void ori(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs].s64() | imm);
}

void pref()
{
}

void sb(u32 rs, u32 rt, s16 imm)
{
    virtual_write<1>(gpr[rs].s32() + imm, gpr[rt].s8());
}

void sd(u32 rs, u32 rt, s16 imm)
{
    virtual_write<8>(gpr[rs].s32() + imm, gpr[rt].s64());
}

void sdl(u32 rs, u32 rt, s16 imm)
{
}

void sdr(u32 rs, u32 rt, s16 imm)
{
}

void sh(u32 rs, u32 rt, s16 imm)
{
    virtual_write<2>(gpr[rs].s32() + imm, gpr[rt].s16());
}

void sll(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s32() << sa);
}

void sllv(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s32() << (gpr[rs].s32() & 31));
}

void slt(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, u32(gpr[rs].s64() < gpr[rt].s64()));
}

void slti(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, u32(gpr[rs].s64() < imm));
}

void sltiu(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, u32(gpr[rs].u64() < u64(imm)));
}

void sltu(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, u32(gpr[rs].u64() < gpr[rt].u64()));
}

void sq(u32 rs, u32 rt, s16 imm)
{
    virtual_write<16>((gpr[rs].s32() + imm) & ~15, gpr[rt].m128i());
}

void sra(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s32() >> sa);
}

void srav(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s32() >> (gpr[rs].s32() & 31));
}

void srl(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].u32() >> sa);
}

void srlv(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].u32() >> (gpr[rs].s32() & 31));
}

void sw(u32 rs, u32 rt, s16 imm)
{
    virtual_write<4>(gpr[rs].s32() + imm, gpr[rt].s32());
}

void swl(u32 rs, u32 rt, s16 imm)
{
}

void swr(u32 rs, u32 rt, s16 imm)
{
}

void sync()
{
}

void syscall()
{
    syscall_exception();
}

void sub(u32 rs, u32 rt, u32 rd)
{
    s32 sum;
    bool overflow;
#if USE_BUILTIN_SUB_OVERFLOW
    overflow = __builtin_sub_overflow(gpr[rs].s32(), gpr[rt].s32(), &sum);
#else
    sum = gpr[rs].s32() - gpr[rt].s32();
    overflow = (gpr[rs].s32() ^ gpr[rt].s32()) & ~(gpr[rt].s32() ^ sum) & 0x8000'0000;
#endif
    if (overflow) {
        integer_overflow_exception();
    } else {
        gpr.set(rd, sum);
    }
}

void subu(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s32() - gpr[rt].s32());
}

void teq(u32 rs, u32 rt)
{
    if (gpr[rs].s64() == gpr[rt].s64()) {
        trap_exception();
    }
}

void teqi(u32 rs, s16 imm)
{
    if (gpr[rs].s64() == imm) {
        trap_exception();
    }
}

void tge(u32 rs, u32 rt)
{
    if (gpr[rs].s64() >= gpr[rt].s64()) {
        trap_exception();
    }
}

void tgei(u32 rs, s16 imm)
{
    if (gpr[rs].s64() >= imm) {
        trap_exception();
    }
}

void tgeu(u32 rs, u32 rt)
{
    if (gpr[rs].u64() >= gpr[rt].u64()) {
        trap_exception();
    }
}

void tgeiu(u32 rs, s16 imm)
{
    if (gpr[rs].u64() >= u64(imm)) {
        trap_exception();
    }
}

void tlt(u32 rs, u32 rt)
{
    if (gpr[rs].s64() < gpr[rt].s64()) {
        trap_exception();
    }
}

void tlti(u32 rs, s16 imm)
{
    if (gpr[rs].s64() < imm) {
        trap_exception();
    }
}

void tltu(u32 rs, u32 rt)
{
    if (gpr[rs].u64() < gpr[rt].u64()) {
        trap_exception();
    }
}

void tltiu(u32 rs, s16 imm)
{
    if (gpr[rs].u64() < u64(imm)) {
        trap_exception();
    }
}

void tne(u32 rs, u32 rt)
{
    if (gpr[rs].s64() != gpr[rt].s64()) {
        trap_exception();
    }
}

void tnei(u32 rs, s16 imm)
{
    if (gpr[rs].s64() != imm) {
        trap_exception();
    }
}

void xor_(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() ^ gpr[rt].s64());
}

void xori(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs].s64() ^ imm);
}

} // namespace ee::interpreter
