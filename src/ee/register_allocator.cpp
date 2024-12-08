#include "register_allocator.hpp"
#include "asmjit/x86/x86operand.h"
#include "ee/ee.hpp"
#include "jit.hpp"
#include "jit_utils.hpp"
#include "mips/disassembler.hpp"
#include "numtypes.hpp"
#include "platform.hpp"

#include <algorithm>
#include <cassert>
#include <format>
#include <limits>

using namespace asmjit;

namespace ee {

constexpr int register_stack_space = 8 * reg_alloc_nonvolatile_gprs.size();

static s32 get_nonvolatile_host_gpr_stack_offset(HostGpr64 const& gpr)
{
    auto it = std::find(reg_alloc_nonvolatile_gprs.begin(), reg_alloc_nonvolatile_gprs.end(), gpr);
    assert(it != reg_alloc_nonvolatile_gprs.end());
    return 8 * (s32)std::distance(reg_alloc_nonvolatile_gprs.begin(), it);
}

enum : u32 {
    lo_index = 32,
    hi_index,
};

RegisterAllocator::RegisterAllocator()
{
    std::ranges::transform(reg_alloc_volatile_gprs, gpr_bindings.begin(), [](HostGpr64 gpr) {
        return Binding{ .host = gpr, .is_volatile = true };
    });
    std::ranges::transform(reg_alloc_nonvolatile_gprs,
      gpr_bindings.begin() + reg_alloc_volatile_gprs.size(),
      [](HostGpr64 gpr) { return Binding{ .host = gpr, .is_volatile = false }; });
}

void RegisterAllocator::BlockEpilog()
{
    FlushAndRestoreAll();
    if constexpr (platform.a64) {}
    if constexpr (platform.x64) {
        if (nonvolatile_gprs_used) {
            c.add(x86::rsp, register_stack_space);
        }
        if constexpr (!IsVolatile(guest_gpr_base_ptr_reg)) {
            c.pop(guest_gpr_base_ptr_reg);
            stack_is_aligned_for_call = !stack_is_aligned_for_call;
        }
        c.ret();
    }
}

void RegisterAllocator::BlockEpilogWithJmp(void* func)
{
    FlushAndRestoreAll();
    if constexpr (!IsVolatile(guest_gpr_base_ptr_reg)) {
        c.pop(guest_gpr_base_ptr_reg);
        stack_is_aligned_for_call = !stack_is_aligned_for_call;
    }
    if constexpr (platform.a64) {}
    if constexpr (platform.x64) {
        if (nonvolatile_gprs_used) {
            c.add(x86::rsp, register_stack_space);
        }
        if constexpr (!IsVolatile(guest_gpr_base_ptr_reg)) {
            c.pop(guest_gpr_base_ptr_reg);
            stack_is_aligned_for_call = !stack_is_aligned_for_call;
        }
        c.jmp(func);
    }
}

void RegisterAllocator::BlockProlog()
{
    Reset();
    auto gpr_mid_ptr = gpr.data() + gpr.size() / 2;
    if constexpr (platform.a64) {}
    if constexpr (platform.x64) {
        if constexpr (!IsVolatile(guest_gpr_base_ptr_reg)) {
            c.push(guest_gpr_base_ptr_reg);
            stack_is_aligned_for_call = !stack_is_aligned_for_call;
        }
        c.mov(guest_gpr_base_ptr_reg, gpr_mid_ptr);
    }
}

void RegisterAllocator::Flush(Binding const& b, bool restore) const
{
    if (!b.Occupied()) return;
    if (b.dirty) {
        s32 offset = GetGprMidPtrOffset(b.guest.value());
        if constexpr (platform.a64) {}
        if constexpr (platform.x64) {
            c.mov(qword_ptr(guest_gpr_base_ptr_reg, offset), b.host);
        }
    }
    if (!b.is_volatile && restore) {
        RestoreHost(b.host);
    }
}

void RegisterAllocator::FlushAll()
{
    for (Binding& binding : gpr_bindings) {
        Flush(binding, false);
    }
}

void RegisterAllocator::FlushAndDestroyAllVolatile()
{
    for (Binding& binding : gpr_bindings) {
        if (binding.is_volatile) {
            FlushAndDestroyBinding(binding, false);
        }
    }
}

void RegisterAllocator::FlushAndDestroyBinding(Binding& b, bool restore)
{
    Flush(b, restore);
    ResetBinding(b);
}

// This should only be used as part of an instruction epilogue. Thus, there is no need
// to destroy bindings. In fact, this would be undesirable, since this function could not
// be called in an epilog emitted mid-block, as part of a code path dependent on a run-time branch.
void RegisterAllocator::FlushAndRestoreAll() const
{
    for (Binding const& binding : gpr_bindings) {
        Flush(binding, true);
    }
}

HostGpr64 RegisterAllocator::GetDirtyGpr(u32 guest)
{
    return GetGpr(guest, guest != 0);
}

HostGpr64 RegisterAllocator::GetGpr(u32 guest)
{
    return GetGpr(guest, false);
}

HostGpr64 RegisterAllocator::GetGpr(u32 guest, bool make_dirty)
{
    Binding* binding = guest_to_host[guest];
    if (binding) {
        binding->access_index = host_access_index++;
        binding->dirty |= make_dirty;
        return binding->host;
    }

    bool found_free{};

    if (next_free_binding_it != gpr_bindings.end()) {
        binding = next_free_binding_it++;
        found_free = true;
    } else {
        auto min_access_index = std::numeric_limits<decltype(host_access_index)>::max();
        for (Binding& b : gpr_bindings) {
            if (!b.Occupied()) {
                found_free = true;
                binding = &b;
                break;
            } else if (b.access_index < min_access_index) {
                min_access_index = b.access_index;
                binding = &b;
            }
        }
        assert(binding);
        if (!found_free) {
            FlushAndDestroyBinding(*binding, false);
        }
    }

    binding->guest = u8(guest);
    binding->access_index = host_access_index++;
    binding->dirty = make_dirty;
    guest_to_host[guest] = binding;

    HostGpr64 const& host = binding->host;

    if (!binding->is_volatile) {
        if (!std::exchange(nonvolatile_gprs_used, true)) {
            if constexpr (platform.a64) {
                // TODO
            }
            if constexpr (platform.x64) {
                c.sub(x86::rsp, register_stack_space);
            }
        }
        if (found_free) {
            s32 stack_offset = get_nonvolatile_host_gpr_stack_offset(host);
            if constexpr (platform.a64) {
                // TODO
            }
            if constexpr (platform.x64) {
                c.mov(qword_ptr(x86::rsp, stack_offset), host);
            }
        }
    }

    if constexpr (platform.a64) {
        if (guest == 0) {
            c.mov(host, 0);
        } else {
            // TODO
        }
    }
    if constexpr (platform.x64) {
        if (guest == 0) {
            c.xor_(host.r32(), host.r32());
        } else {
            c.mov(host, qword_ptr(guest_gpr_base_ptr_reg, GetGprMidPtrOffset(guest)));
        }
    }

    return host;
}

s32 RegisterAllocator::GetGprMidPtrOffset(u32 guest) const
{
    static constexpr u32 kNumRegs = 32;
    return s32(sizeof(u128)) * (guest - kNumRegs / 2);
}

std::string RegisterAllocator::GetStatus() const
{
    std::string used_str, free_str;
    for (Binding const& b : gpr_bindings) {
        auto host_reg_str{ JitRegToStr(b.host) };
        if (b.Occupied()) {
            u32 guest = b.guest.value();
            std::string guest_reg_str =
              b.host.isXmm() ? std::format("$v{}", guest) : std::string(mips::gpr_index_to_name(guest));
            used_str.append(std::format("{}({},{}),", host_reg_str, guest_reg_str, b.dirty ? 'd' : 'c'));
        } else {
            free_str.append(host_reg_str);
            free_str.push_back(',');
        }
    }
    return std::format("Used: {}; Free: {}\n", used_str, free_str);
}

void RegisterAllocator::Reset()
{
    for (Binding& b : gpr_bindings) {
        b.access_index = 0;
        b.dirty = false;
        b.guest = {};
    }
    guest_to_host = {};
    host_access_index = 0;
    next_free_binding_it = gpr_bindings.begin();
    nonvolatile_gprs_used = false;
    stack_is_aligned_for_call = false;
}

void RegisterAllocator::ResetBinding(Binding& b)
{
    if (b.Occupied()) {
        guest_to_host[b.guest.value()] = {};
        b.guest = {};
        b.dirty = false;
    }
    b.access_index = host_access_index;
}

void RegisterAllocator::RestoreHost(HostGpr64 gpr) const
{
    if constexpr (platform.a64) {}
    if constexpr (platform.x64) {
        c.mov(gpr, qword_ptr(x86::rsp, 8 * gpr.id()));
    }
}

void RegisterAllocator::SaveHost(HostGpr64 gpr) const
{
    if constexpr (platform.a64) {}
    if constexpr (platform.x64) {
        c.mov(qword_ptr(x86::rsp, 8 * gpr.id()), gpr);
    }
}

bool RegisterAllocator::StackIsAlignedForCall() const
{
    return stack_is_aligned_for_call;
}

} // namespace ee
