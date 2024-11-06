#pragma once

#include "algorithm.hpp"
#include "always_false.hpp"
#include "asmjit/a64.h"
#include "asmjit/x86.h"
#include "asmjit/x86/x86operand.h"
#include "build_options.hpp"
#include "log.hpp"
#include "numtypes.hpp"
#include "platform.hpp"

#include <array>
#include <bit>
#include <cassert>
#include <concepts>
#include <expected>
#include <limits>
#include <print>
#include <string>
#include <type_traits>

using HostGpr32 = std::conditional_t<platform.x64, asmjit::x86::Gpd, asmjit::a64::GpW>;
using HostGpr64 = std::conditional_t<platform.x64, asmjit::x86::Gpq, asmjit::a64::GpX>;
using HostGpr128 = std::conditional_t<platform.x64, asmjit::x86::Xmm, asmjit::a64::VecV>;
using JitCompiler = std::conditional_t<platform.x64, asmjit::x86::Compiler, asmjit::a64::Compiler>;

struct AsmjitLogErrorHandler : public asmjit::ErrorHandler {
    void handleError(asmjit::Error err, char const* message, asmjit::BaseEmitter* /*origin*/) override
    {
        std::println("AsmJit error {}: {}", err, message);
    }
};

inline constexpr std::array host_gpr_arg = [] {
    if constexpr (platform.a64) {
        using namespace asmjit::a64;
        return std::array{ x0, x1, x2, x3, x4, x5, x6, x7 };
    }
    if constexpr (platform.x64) {
        using namespace asmjit::x86;
        if constexpr (platform.abi.systemv) {
            return std::array{ rdi, rsi, rdx, rcx, r8, r9 };
        }
        if constexpr (platform.abi.win64) {
            return std::array{ rcx, rdx, r8, r9 };
        }
    }
}();

inline void jit_x64_call_with_stack_alignment(asmjit::x86::Compiler& c, auto func)
{
    (void)c;
    (void)func;
    // using namespace asmjit::x86;
    // if constexpr (os.linux) {
    //     c.push(rax);
    //     c.call(func);
    //     c.pop(rcx);
    // } else {
    //     c.sub(rsp, 40);
    //     c.call(func);
    //     c.add(rsp, 40);
    // }
}

inline void jit_x86_call_no_stack_alignment(asmjit::x86::Compiler& c, auto func)
{
    (void)c;
    (void)func;
    // using namespace asmjit::x86;
    // if constexpr (os.linux) {
    //     c.call(func);
    // } else {
    //     c.sub(rsp, 32);
    //     c.call(func);
    //     c.add(rsp, 32);
    // }
}

constexpr std::string_view JitRegToStr(asmjit::x86::Gp gp)
{
    using namespace asmjit::x86;
    switch (gp.id()) {
    case Gp::Id::kIdAx:  return "rax";
    case Gp::Id::kIdCx:  return "rcx";
    case Gp::Id::kIdDx:  return "rdx";
    case Gp::Id::kIdBx:  return "rbx";
    case Gp::Id::kIdSp:  return "rsp";
    case Gp::Id::kIdBp:  return "rbp";
    case Gp::Id::kIdSi:  return "rsi";
    case Gp::Id::kIdDi:  return "rdi";
    case Gp::Id::kIdR8:  return "r8";
    case Gp::Id::kIdR9:  return "r9";
    case Gp::Id::kIdR10: return "r10";
    case Gp::Id::kIdR11: return "r11";
    case Gp::Id::kIdR12: return "r12";
    case Gp::Id::kIdR13: return "r13";
    case Gp::Id::kIdR14: return "r14";
    case Gp::Id::kIdR15: return "r15";
    default:             return "UNKNOWN";
    }
}

inline std::string JitRegToStr(asmjit::x86::Xmm xmm)
{
    return std::format("xmm{}", xmm.id());
}

inline std::string JitRegToStr(asmjit::x86::Ymm ymm)
{
    return std::format("ymm{}", ymm.id());
}

inline std::string JitRegToStr(asmjit::x86::Zmm zmm)
{
    return std::format("zmm{}", zmm.id());
}

inline std::string JitRegToStr(asmjit::a64::Vec vec)
{
    return std::format("v{}", vec.id());
}

constexpr bool IsVolatile(asmjit::a64::Gp gpr)
{
    return gpr.id() < 18;
}

constexpr bool IsVolatile(asmjit::x86::Gp gpr)
{
    using namespace asmjit::x86;
    using GpIdT = std::underlying_type_t<Gp::Id>;
    static constexpr GpIdT mask = [] {
        if constexpr (platform.abi.systemv) {
            // TODO: ensure no overflow
            return 1 << rax.id() | 1 << rcx.id() | 1 << rdx.id() | 1 << rdi.id() | 1 << rsi.id() | 1 << r8.id()
                 | 1 << r9.id() | 1 << r10.id() | 1 << r11.id();
        }
        if constexpr (platform.abi.win64) {
            return 1 << rax.id() | 1 << rcx.id() | 1 << rdx.id() | 1 << r8.id() | 1 << r9.id() | 1 << r10.id()
                 | 1 << r11.id();
        }
    }();
    return mask >> gpr.id() & 1;
}

constexpr bool IsVolatile(asmjit::a64::Vec gpr)
{
    u32 id = gpr.id();
    return id < 8 || id > 15;
}

constexpr bool IsVolatile(asmjit::x86::Vec gpr)
{
    if constexpr (platform.abi.systemv) {
        return true;
    }
    if constexpr (platform.abi.win64) {
        u32 id = gpr.id();
        return id < 6 || id > 15;
    }
}
