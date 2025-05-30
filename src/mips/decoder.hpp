#pragma once

#include "numtypes.hpp"

#include <string>
#include <string_view>
#include <utility>

namespace mips {

constexpr std::string_view gpr_index_to_name(u32 idx)
{
    switch (idx) {
    case 0:  return "$zero";
    case 1:  return "$at";
    case 2:  return "$v0";
    case 3:  return "$v1";
    case 4:  return "$a0";
    case 5:  return "$a1";
    case 6:  return "$a2";
    case 7:  return "$a3";
    case 8:  return "$t0";
    case 9:  return "$t1";
    case 10: return "$t2";
    case 11: return "$t3";
    case 12: return "$t4";
    case 13: return "$t5";
    case 14: return "$t6";
    case 15: return "$t7";
    case 16: return "$s0";
    case 17: return "$s1";
    case 18: return "$s2";
    case 19: return "$s3";
    case 20: return "$s4";
    case 21: return "$s5";
    case 22: return "$s6";
    case 23: return "$s7";
    case 24: return "$t8";
    case 25: return "$t9";
    case 26: return "$k0";
    case 27: return "$k1";
    case 28: return "$gp";
    case 29: return "$sp";
    case 30: return "$fp";
    case 31: return "$ra";
    default: std::unreachable();
    }
}

void decode_ee(u32 instr);
void decode_iop(u32 instr);
std::string disassemble(u32 instr);

} // namespace mips
