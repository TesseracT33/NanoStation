#include "cpu.hpp"
#include "ee.hpp"
#include "exceptions.hpp"
#include "mmu.hpp"

#include <limits>

namespace ee {

using enum mips::CpuImpl;

template<> void add<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    s32 sum;
    bool overflow;
#if HAS_BUILTIN_ADD_OVERFLOW
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

template<> void addi<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 sum;
    bool overflow;
#if HAS_BUILTIN_ADD_OVERFLOW
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

template<> void addiu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs].s32() + imm);
}

template<> void addu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s32() + gpr[rt].s32());
}

template<> void and_<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() & gpr[rt].s64());
}

template<> void andi<Interpreter>(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs].s64() & imm);
}

template<> void beq<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() == gpr[rt].s64()) {
        jump(pc + (imm << 2));
    }
}

template<> void beql<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() == gpr[rt].s64()) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void bgez<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bgezal<Interpreter>(u32 rs, s16 imm)
{
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bgezall<Interpreter>(u32 rs, s16 imm)
{
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void bgezl<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() >= 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void bgtz<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() > 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bgtzl<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() > 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void blez<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() <= 0) {
        jump(pc + (imm << 2));
    }
}

template<> void blezl<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() <= 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void bltz<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bltzal<Interpreter>(u32 rs, s16 imm)
{
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    }
}

template<> void bltzall<Interpreter>(u32 rs, s16 imm)
{
    gpr.set(31, 4 + (in_branch_delay_slot ? jump_addr : pc));
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void bltzl<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() < 0) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void bne<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() != gpr[rt].s64()) {
        jump(pc + (imm << 2));
    }
}

template<> void bnel<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    if (gpr[rs].s64() != gpr[rt].s64()) {
        jump(pc + (imm << 2));
    } else {
        pc += 4;
    }
}

template<> void break_<Interpreter>()
{
    breakpoint_exception();
}

template<> void cache<Interpreter>()
{
    // TODO
}

template<> void dadd<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    s64 sum;
    bool overflow;
#if HAS_BUILTIN_ADD_OVERFLOW
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

template<> void daddi<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s64 sum;
    bool overflow;
#if HAS_BUILTIN_ADD_OVERFLOW
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

template<> void daddiu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs].s64() + imm);
}

template<> void daddu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() + gpr[rt].s64());
}

template<> void div<Interpreter>(u32 rs, u32 rt)
{
    s32 op1 = gpr[rs].s32();
    s32 op2 = gpr[rt].s32();
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
}

template<> void div1<Interpreter>(u32 rs, u32 rt)
{
    s32 op1 = gpr[rs].s32();
    s32 op2 = gpr[rt].s32();
    if (op2 == 0) {
        lo.set_upper_dword(op1 >= 0 ? -1 : 1);
        hi.set_upper_dword(op1);
    } else if (op1 == std::numeric_limits<s32>::min() && op2 == -1) {
        lo.set_upper_dword(std::numeric_limits<s32>::min());
        hi.set_upper_dword(0);
    } else [[likely]] {
        lo.set_upper_dword(op1 / op2);
        hi.set_upper_dword(op1 % op2);
    }
}

template<> void divu<Interpreter>(u32 rs, u32 rt)
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

template<> void divu1<Interpreter>(u32 rs, u32 rt)
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

template<> void dsll<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() << sa);
}

template<> void dsll32<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() << (sa + 32));
}

template<> void dsllv<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s64() << (gpr[rs].s32() & 63));
}

template<> void dsra<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() >> sa);
}

template<> void dsra32<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s64() >> (sa + 32));
}

template<> void dsrav<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s64() >> (gpr[rs].s32() & 63));
}

template<> void dsrl<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].u64() >> sa);
}

template<> void dsrl32<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].u64() >> (sa + 32));
}

template<> void dsrlv<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].u64() >> (gpr[rs].s32() & 63));
}

template<> void dsub<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    s64 sum;
    bool overflow;
#if HAS_BUILTIN_SUB_OVERFLOW
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

template<> void dsubu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() - gpr[rt].s64());
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
        s32 target = gpr[rs].s32();
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
        s32 target = gpr[rs].s32();
        if (target & 3) {
            address_error_exception(target, MemOp::InstrFetch);
        } else {
            jump(target);
        }
    }
}

template<> void lb<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s8 val = virtual_read<u8>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lbu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u8 val = virtual_read<u8>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void ld<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s64 val = virtual_read<u64>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void ldl<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s64 val = virtual_read<u64, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s64 load_mask = s64(0xFF00'0000'0000'0000) >> (8 * (addr & 7));
        gpr.set(rt, val & load_mask | gpr[rt].s64() & ~load_mask);
    }
}

template<> void ldr<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s64 val = virtual_read<u64, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 bits_offset = 8 * (addr & 7);
        u64 load_mask = 0xFFFF'FFFF'FFFF'FFFF >> bits_offset;
        val >>= bits_offset;
        gpr.set(rt, val & load_mask | gpr[rt].s64() & ~load_mask);
    }
}

template<> void lh<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s16 val = virtual_read<u16>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lhu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u16 val = virtual_read<u16>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lq<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u128 val = virtual_read<u128>((gpr[rs].s32() + imm) & ~15);
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
    s32 val = virtual_read<u32>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void lwl<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s32 val = virtual_read<u32, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 load_mask = s32(0xFF00'0000) >> (8 * (addr & 3));
        gpr.set(rt, val & load_mask | gpr[rt].s32() & ~load_mask);
    }
}

template<> void lwr<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    s32 addr = gpr[rs].s32() + imm;
    s32 val = virtual_read<u32, Alignment::Unaligned>(addr);
    if (!exception_occurred) {
        s32 bits_offset = 8 * (addr & 3);
        u32 load_mask = 0xFFFF'FFFF >> bits_offset;
        val >>= bits_offset;
        gpr.set(rt, val & load_mask | gpr[rt].s32() & ~load_mask);
    }
}

template<> void lwu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    u32 val = virtual_read<u32>(gpr[rs].s32() + imm);
    if (!exception_occurred) {
        gpr.set(rt, val);
    }
}

template<> void mfhi<Interpreter>(u32 rd)
{
    gpr.set(rd, hi);
}

template<> void mfhi1<Interpreter>(u32 rd)
{
    gpr.set(rd, hi.get_upper_dword());
}

template<> void mflo<Interpreter>(u32 rd)
{
    gpr.set(rd, lo);
}

template<> void mflo1<Interpreter>(u32 rd)
{
    gpr.set(rd, lo.get_upper_dword());
}

template<> void mfsa<Interpreter>(u32 rd)
{
    gpr.set(rd, sa);
}

template<> void movn<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    if (gpr[rt].s64() != 0) {
        gpr.set(rd, gpr[rs].s64());
    }
}

template<> void movz<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    if (gpr[rt].s64() == 0) {
        gpr.set(rd, gpr[rs].s64());
    }
}

template<> void mthi<Interpreter>(u32 rs)
{
    hi = gpr[rs].s64();
}

template<> void mthi1<Interpreter>(u32 rs)
{
    hi.set_upper_dword(gpr[rs].s64());
}

template<> void mtlo<Interpreter>(u32 rs)
{
    lo = gpr[rs].s64();
}

template<> void mtlo1<Interpreter>(u32 rs)
{
    lo.set_upper_dword(gpr[rs].s64());
}

template<> void mtsa<Interpreter>(u32 rs)
{
    sa = gpr[rs].u32();
}

template<> void mtsab<Interpreter>(u32 rs, s16 imm)
{
    sa = 8 * (15 & (gpr[rs].s16() ^ imm));
}

template<> void mtsah<Interpreter>(u32 rs, s16 imm)
{
    sa = 16 * (7 & (gpr[rs].s16() ^ imm));
}

template<> void mult<Interpreter>(u32 rs, u32 rt)
{
    s64 prod = s64(gpr[rs].s32()) * s64(gpr[rt].s32());
    lo = s32(prod);
    hi = prod >> 32;
}

template<> void mult1<Interpreter>(u32 rs, u32 rt)
{
    s64 prod = s64(gpr[rs].s32()) * s64(gpr[rt].s32());
    lo.set_upper_dword(s32(prod));
    hi.set_upper_dword(prod >> 32);
}

template<> void multu<Interpreter>(u32 rs, u32 rt)
{
    u64 prod = u64(gpr[rs].u32()) * u64(gpr[rt].u32());
    lo = s32(prod);
    hi = s32(prod >> 32);
}

template<> void multu1<Interpreter>(u32 rs, u32 rt)
{
    u64 prod = u64(gpr[rs].u32()) * u64(gpr[rt].u32());
    lo.set_upper_dword(s32(prod));
    hi.set_upper_dword(s32(prod >> 32));
}

template<> void nor<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, ~(gpr[rs].s64() | gpr[rt].s64()));
}

template<> void or_<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() | gpr[rt].s64());
}

template<> void ori<Interpreter>(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs].s64() | imm);
}

template<> void pref<Interpreter>()
{
}

template<> void sb<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    virtual_write<1>(gpr[rs].s32() + imm, gpr[rt].s8());
}

template<> void sd<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    virtual_write<8>(gpr[rs].s32() + imm, gpr[rt].s64());
}

template<> void sdl<Interpreter>(u32 rs, u32 rt, s16 imm)
{
}

template<> void sdr<Interpreter>(u32 rs, u32 rt, s16 imm)
{
}

template<> void sh<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    virtual_write<2>(gpr[rs].s32() + imm, gpr[rt].s16());
}

template<> void sll<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s32() << sa);
}

template<> void sllv<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s32() << (gpr[rs].s32() & 31));
}

template<> void slt<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() < gpr[rt].s64());
}

template<> void slti<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs].s64() < imm);
}

template<> void sltiu<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    gpr.set(rt, gpr[rs].u64() < u64(imm));
}

template<> void sltu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].u64() < gpr[rt].u64());
}

template<> void sq<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    virtual_write<16>((gpr[rs].s32() + imm) & ~15, gpr[rt].m128i());
}

template<> void sra<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].s32() >> sa);
}

template<> void srav<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].s32() >> (gpr[rs].s32() & 31));
}

template<> void srl<Interpreter>(u32 rt, u32 rd, u32 sa)
{
    gpr.set(rd, gpr[rt].u32() >> sa);
}

template<> void srlv<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rt].u32() >> (gpr[rs].s32() & 31));
}

template<> void sw<Interpreter>(u32 rs, u32 rt, s16 imm)
{
    virtual_write<4>(gpr[rs].s32() + imm, gpr[rt].s32());
}

template<> void swl<Interpreter>(u32 rs, u32 rt, s16 imm)
{
}

template<> void swr<Interpreter>(u32 rs, u32 rt, s16 imm)
{
}

template<> void sync<Interpreter>()
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

template<> void subu<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s32() - gpr[rt].s32());
}

template<> void teq<Interpreter>(u32 rs, u32 rt)
{
    if (gpr[rs].s64() == gpr[rt].s64()) {
        trap_exception();
    }
}

template<> void teqi<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() == imm) {
        trap_exception();
    }
}

template<> void tge<Interpreter>(u32 rs, u32 rt)
{
    if (gpr[rs].s64() >= gpr[rt].s64()) {
        trap_exception();
    }
}

template<> void tgei<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() >= imm) {
        trap_exception();
    }
}

template<> void tgeu<Interpreter>(u32 rs, u32 rt)
{
    if (gpr[rs].u64() >= gpr[rt].u64()) {
        trap_exception();
    }
}

template<> void tgeiu<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].u64() >= u64(imm)) {
        trap_exception();
    }
}

template<> void tlt<Interpreter>(u32 rs, u32 rt)
{
    if (gpr[rs].s64() < gpr[rt].s64()) {
        trap_exception();
    }
}

template<> void tlti<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() < imm) {
        trap_exception();
    }
}

template<> void tltu<Interpreter>(u32 rs, u32 rt)
{
    if (gpr[rs].u64() < gpr[rt].u64()) {
        trap_exception();
    }
}

template<> void tltiu<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].u64() < u64(imm)) {
        trap_exception();
    }
}

template<> void tne<Interpreter>(u32 rs, u32 rt)
{
    if (gpr[rs].s64() != gpr[rt].s64()) {
        trap_exception();
    }
}

template<> void tnei<Interpreter>(u32 rs, s16 imm)
{
    if (gpr[rs].s64() != imm) {
        trap_exception();
    }
}

template<> void xor_<Interpreter>(u32 rs, u32 rt, u32 rd)
{
    gpr.set(rd, gpr[rs].s64() ^ gpr[rt].s64());
}

template<> void xori<Interpreter>(u32 rs, u32 rt, u16 imm)
{
    gpr.set(rt, gpr[rs].s64() ^ imm);
}

} // namespace ee
