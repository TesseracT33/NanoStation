#pragma once

#include "asmjit/a64.h"
#include "asmjit/x86.h"
#include "asmjit/x86/x86operand.h"
#include "jit_utils.hpp"
#include "numtypes.hpp"
#include "platform.hpp"

#include <array>
#include <optional>
#include <string>

namespace ee {

inline constexpr std::array reg_alloc_volatile_gprs = [] {
    if constexpr (platform.a64) {
        using namespace asmjit::a64;
        return std::array{ x0, x1, x2, x3, x4, x5, x6, x7, x8, x9, x10, x11, x12, x13, x14, x15, x16 };
    }
    if constexpr (platform.x64) {
        using namespace asmjit::x86;
        if constexpr (platform.abi.systemv) {
            return std::array{ rdi, rsi, rdx, rcx, r8, r9, r10, r11 };
        }
        if constexpr (platform.abi.win64) {
            return std::array{ rdx, rcx, r8, r9, r10, r11 };
        }
    }
}();

inline constexpr std::array reg_alloc_volatile_vprs = [] {
    if constexpr (platform.a64) {
        using namespace asmjit::a64;
        return std::array{
            v0,
            v1,
            v2,
            v3,
            v4,
            v5,
            v6,
            v7,
            v16,
            v17,
            v18,
            v19,
            v20,
            v21,
            v22,
            v23,
            v24,
            v25,
            v26,
            v27,
            v28,
            v29,
            v30,
            v31,
        };
    }
    if constexpr (platform.x64) {
        using namespace asmjit::x86;
        if constexpr (platform.abi.systemv) {
            return std::array{
                xmm0,
                xmm1,
                xmm2,
                xmm3,
                xmm4,
                xmm5,
                xmm6,
                xmm7,
                xmm8,
                xmm9,
                xmm10,
                xmm11,
                xmm12,
                xmm13,
                xmm14,
                xmm15,
            };
        }
        if constexpr (platform.abi.win64) {
            return std::array{ xmm0, xmm1, xmm2, xmm3, xmm4, xmm5 };
        }
    }
}();

inline constexpr std::array reg_alloc_nonvolatile_gprs = [] {
    if constexpr (platform.a64) {
        using namespace asmjit::a64;
        return std::array{ x18, x19, x20, x21, x22, x23, x24, x25, x26, x27, x28 };
    }
    if constexpr (platform.x64) {
        using namespace asmjit::x86;
        if constexpr (platform.abi.systemv) {
            return std::array{ rbx, r12, r13, r14, r15 };
        }
        if constexpr (platform.abi.win64) {
            return std::array{ rbx, r12, r13, r14, r15, rdi, rsi };
        }
    }
}();

inline constexpr std::array reg_alloc_nonvolatile_vprs = [] {
    if constexpr (platform.a64) {
        using namespace asmjit::a64;
        return std::array{ v8, v9, v10, v11, v12, v13, v14, v15 };
    }
    if constexpr (platform.x64) {
        using namespace asmjit::x86;
        if constexpr (platform.abi.systemv) {
            return std::array<Xmm, 0>{};
        }
        if constexpr (platform.abi.win64) {
            return std::array{ xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15 };
        }
    }
}();

inline constexpr HostGpr64 guest_gpr_base_ptr_reg = [] {
    if constexpr (platform.a64) return asmjit::a64::x17;
    if constexpr (platform.x64) return asmjit::x86::rbp;
}();

inline constexpr size_t reg_alloc_num_gprs = reg_alloc_volatile_gprs.size() + reg_alloc_nonvolatile_gprs.size();
inline constexpr size_t reg_alloc_num_vprs = reg_alloc_volatile_vprs.size() + reg_alloc_nonvolatile_vprs.size();

class RegisterAllocator {

    struct Binding {
        HostGpr64 host;
        std::optional<u32> guest;
        u16 access_index;
        bool dirty;
        bool is_volatile;
        bool Occupied() const { return guest.has_value(); }
    };

public:
    RegisterAllocator();

    void BlockEpilog();
    void BlockEpilogWithJmp(void* func);
    void BlockProlog();
    void FlushAll();
    HostGpr64 GetDirtyHostGpr(u32 guest);
    HostGpr64 GetHostGpr(u32 guest);
    std::string GetStatusString() const;
    bool StackIsAlignedForCall() const;

protected:
    std::array<Binding, reg_alloc_num_gprs> gpr_bindings;
    std::array<Binding, reg_alloc_num_vprs> vpr_bindings;
    std::array<Binding*, 32> guest_to_host;
    std::array<Binding*, 32> guest128_to_host;
    typename decltype(gpr_bindings)::iterator next_free_binding_it{ gpr_bindings.begin() };
    u16 host_access_index{};
    bool nonvolatile_gprs_used{};
    bool stack_is_aligned_for_call{};

    void Flush(Binding const& b, bool restore) const;
    void FlushAndDestroyAllVolatile();
    void FlushAndDestroyBinding(Binding& b, bool restore, bool keep_reserved);
    // This should only be used as part of an instruction epilogue. Thus, there is no need
    // to destroy bindings. In fact, this would be undesirable, since this function could not
    // be called in an epilog emitted mid-block, as part of a code path dependent on a run-time branch.
    void FlushAndRestoreAll() const;
    HostGpr64 GetHostGpr(u32 guest, bool make_dirty);
    HostGpr128 GetHostVpr(u32 guest, bool make_dirty);
    void Load(Binding& binding, bool save);
    void Reset();
    void ResetBinding(Binding& b, bool keep_reserved);
    void RestoreHost(HostGpr64 gpr) const;
    void SaveHost(HostGpr64 gpr) const;
};

} // namespace ee