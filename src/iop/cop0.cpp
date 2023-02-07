#include "cop0.hpp"
#include "exceptions.hpp"
#include "iop.hpp"

#include <bit>
#include <cstring>

namespace iop {

using enum mips::CpuImpl;

template<> void mfc0<Interpreter>(u32 rd, u32 rt)
{
    if (rd < 16 || rd > 31) [[likely]] { // TODO: can we really read above 31? if so, adjust function argument RD
        static constexpr u64 reserved = 0xFFFF'FFFF'0000'0417; // 0-2, 4, 10, 32-63
        if (reserved & (1ll << rd)) [[unlikely]] {
            reserved_instruction_exception();
        } else {
            static_assert(sizeof(cop0) == 16 * 4);
            u32 val;
            std::memcpy(&val, reinterpret_cast<u8*>(&cop0) + rd * 4, 4);
            gpr.set(rt, val);
        }
    } else {
        gpr.set(rt, 0); // "garbage"
    }
}

template<> void mtc0<Interpreter>(u32 rd, u32 rt)
{
    if (rd > 15) return;

    switch (rd) {
    case 3: cop0.bpc = gpr[rt]; break;
    case 5: cop0.bda = gpr[rt]; break;
    case 9: cop0.bdam = gpr[rt]; break;
    case 11: cop0.bpcm = gpr[rt]; break;
    case 12: cop0.status.raw = gpr[rt] & 0xF4C7'9C1F | cop0.status.raw & ~0xF4C7'9C1F; break; // TODO: determine mask
    case 13: cop0.cause.raw = gpr[rt] & 0x300 | cop0.cause.raw & ~0x300; break;

    case 7: {
        u32 val = gpr[rt]; // TODO: determine mask
        std::memcpy(&cop0.dcic, &val, 4);
    } break;

    case 6:
    case 8:
    case 14:
    case 15: break; // read-only

    default: reserved_instruction_exception(); // TODO: should this happen for writes?
    }
}

template<> void rfe<Interpreter>()
{
    cop0.status.raw = cop0.status.raw >> 2 & 15 | cop0.status.raw & ~15;
}

} // namespace iop
