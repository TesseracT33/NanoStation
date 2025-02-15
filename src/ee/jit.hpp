#pragma once

#include "asmjit/a64.h"
#include "asmjit/x86.h"
#include "ee.hpp"
#include "jit_common.hpp"
#include "numtypes.hpp"
#include "platform.hpp"
#include "register_allocator.hpp"
#include "status.hpp"

#include <type_traits>

namespace ee {

void BlockEpilog();
void BlockEpilogWithJmp(void (*func)());
void BlockEpilogWithPcFlushAndJmp(void (*func)(), int pc_offset = 0);
void BlockEpilogWithPcFlush(int pc_offset = 0);
void BlockProlog();
void RecordBlockCycles();
void DiscardBranch();
bool CheckDwordOpCondJit();
void EmitLink(u32 reg);
void FlushPc(int pc_offset = 0);
Status InitJit();
void Invalidate(u32 paddr);
void InvalidateRange(u32 paddr_lo, u32 paddr_hi);
void OnBranchNotTaken();
u32 RunJit(u32 cpu_cycles);
template<typename Target>
void TakeBranch(Target target)
    requires(std::same_as<Target, u32> || std::same_as<Target, HostGpr32>);
void TearDownJit();

inline JitCompiler c;
inline RegisterAllocator reg_alloc{ c };
inline u32 jit_pc;
inline u32 block_cycles;
inline mips::BranchState branch_state;
inline bool branch_hit;
inline bool branched;
inline bool compiler_exception_occurred;

inline ptrdiff_t get_offset_to_guest_gpr_base_ptr(void const* obj)
{
    return static_cast<u8 const*>(obj) - reinterpret_cast<u8 const*>(&gpr) + sizeof(u128) * 16;
}

template<typename T> asmjit::x86::Mem JitPtr(T const& obj, u32 ptr_size = sizeof(std::remove_pointer_t<T>))
{
    auto obj_ptr = [&] {
        if constexpr (std::is_pointer_v<T>) return obj;
        else return &obj;
    }();
    ptrdiff_t diff = get_offset_to_guest_gpr_base_ptr(obj_ptr);
    return asmjit::x86::ptr(guest_gpr_base_ptr_reg, s32(diff), ptr_size);
}

template<typename T>
asmjit::x86::Mem JitPtrOffset(T const& obj, s32 offset, u32 ptr_size = sizeof(std::remove_pointer_t<T>))
{
    auto obj_ptr = [&] {
        if constexpr (std::is_pointer_v<T>) return obj;
        else return &obj;
    }();
    ptrdiff_t diff = get_offset_to_guest_gpr_base_ptr(obj_ptr) + offset;
    return asmjit::x86::ptr(guest_gpr_base_ptr_reg, s32(diff), ptr_size);
}

template<typename T>
asmjit::x86::Mem JitPtrOffset(T const& obj, asmjit::x86::Gp index, u32 ptr_size = sizeof(std::remove_pointer_t<T>))
{
    auto obj_ptr = [&] {
        if constexpr (std::is_pointer_v<T>) return obj;
        else return &obj;
    }();
    ptrdiff_t diff = reinterpret_cast<u8 const*>(obj_ptr) - reinterpret_cast<u8 const*>(&gpr);
    return asmjit::x86::ptr(guest_gpr_base_ptr_reg, index.r64(), 0u, s32(diff), ptr_size);
}

} // namespace ee
