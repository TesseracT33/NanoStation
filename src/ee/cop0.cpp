#include "cop0.hpp"
#include "ee.hpp"

#include <array>
#include <bit>
#include <cstring>
#include <utility>

namespace ee {
Cop0Registers cop0;

void bc0f(s16 imm)
{
}

void bc0fl(s16 imm)
{
}

void bc0t(s16 imm)
{
}

void bc0tl(s16 imm)
{
}

void di()
{
    u32 edi_erl_exi = std::bit_cast<u32>(cop0.status) & 0x20006;
    if ((cop0.status.ksu == 0) | edi_erl_exi) {
        cop0.status.eie = 0;
    }
}

void ei()
{
    u32 edi_erl_exi = std::bit_cast<u32>(cop0.status) & 0x20006;
    if ((cop0.status.ksu == 0) | edi_erl_exi) {
        cop0.status.eie = 1;
    }
}

void eret()
{
    if (cop0.status.erl) {
        pc = cop0.error_epc;
        cop0.status.erl = 0;
    } else {
        pc = cop0.epc;
        cop0.status.exl = 0;
    }
}

void mfc0(u32 rd, u32 rt)
{
    gpr.set(rt, s32(cop0.get(rd)));
}

void mtc0(u32 rd, u32 rt)
{
    cop0.set(rd, gpr[rt].u32());
}

void tlbp()
{
}

void tlbr()
{
}

void tlbwi()
{
}

void tlbwr()
{
}

u32 Cop0Registers::get(int reg) const
{
    // TODO: update 'random'
    u32 ret;
    std::memcpy(&ret, reinterpret_cast<u8*>(&cop0) + reg * 4, 4);
    return ret;
}

template<bool raw> void Cop0Registers::set(int reg, u32 value)
{
    auto write = [value]<typename Reg>(Reg& reg) { reg = std::bit_cast<Reg>(value); };

    auto write_masked = [value]<typename Reg>(Reg& reg, u32 mask) mutable {
        u32 reg_u32 = std::bit_cast<u32>(reg);
        value = value & mask | reg_u32 & ~mask;
        reg = std::bit_cast<Reg>(value);
    };

    switch (reg) {
    case 0:
        if constexpr (raw) write(index);
        else write_masked(index, 0x8000'003F);
        break;

    case 1:
        if constexpr (raw) write(random);
        break;

    case 2:
        if constexpr (raw) write(entry_lo[0]);
        else write_masked(entry_lo[0], 0x3FFF'FFFF);
        break;

    case 3:
        if constexpr (raw) write(entry_lo[1]);
        else write_masked(entry_lo[1], 0x3FFF'FFFF);
        break;

    case 4:
        if constexpr (raw) write(context);
        else write_masked(context, 0xFF80'0000);
        break;

    case 5:
        if constexpr (raw) write(page_mask);
        else write_masked(page_mask, 0x01FF'E000);
        break;

    case 6:
        if constexpr (raw) write(wired);
        else write_masked(wired, 0x3F);
        on_write_to_wired();
        break;

    case 7: unused_7 = value; break;

    case 8:
        if constexpr (raw) bad_v_addr = value;
        break;

    case 9:
        count = value;
        on_write_to_count();
        break;

    case 10:
        if constexpr (raw) write(entry_hi);
        else write_masked(entry_hi, 0xFFFF'E0FF);
        break;

    case 11:
        compare = value;
        on_write_to_compare();
        break;

    case 12:
        if constexpr (raw) write(status);
        else write_masked(status, 0xF4C7'9C1F);
        on_write_to_status();
        break;

    case 13:
        if constexpr (raw) write(cause);
        on_write_to_cause();
        break;

    case 14: epc = value; break;

    case 15: break;

    case 16:
        if constexpr (raw) write(config);
        else write_masked(cause, 0x73003);
        break;

    case 17: unused_17 = value; break;
    case 18: unused_18 = value; break;
    case 19: unused_19 = value; break;
    case 20: unused_20 = value; break;
    case 21: unused_21 = value; break;
    case 22: unused_22 = value; break;

    case 23: break;

    case 24:
        if constexpr (raw) write(debug);
        else write_masked(debug, 0xF00'800F);
        break;

    case 25:
        if constexpr (raw) write(perf_pccr);
        else write_masked(perf_pccr, 0xF00'800F);
        break;

    case 26: unused_26 = value; break;

    case 27: unused_27 = value; break;

    case 28: write(tag_lo); break;

    case 29: tag_hi = value; break;

    case 30: error_epc = value; break;

    case 31: unused_31 = value; break;

    default: std::unreachable();
    }
}

void Cop0Registers::on_write_to_cause()
{
}

void Cop0Registers::on_write_to_compare()
{
}

void Cop0Registers::on_write_to_count()
{
}

void Cop0Registers::on_write_to_status()
{
}

void Cop0Registers::on_write_to_wired()
{
}

} // namespace ee
