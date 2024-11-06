#pragma once

#include "asmjit/a64.h"
#include "asmjit/x86.h"
#include "ee.hpp"
#include "jit_utils.hpp"
#include "numtypes.hpp"
#include "platform.hpp"
#include "register_allocator.hpp"
#include "status.hpp"

#include <type_traits>

namespace ee {

void BlockEpilog();
void BlockEpilogWithJmp(void* func);
void BlockEpilogWithPcFlushAndJmp(void* func, int pc_offset = 0);
void BlockEpilogWithPcFlush(int pc_offset = 0);
void BlockProlog();
void BlockRecordCycles();
void DiscardBranchJit();
bool CheckDwordOpCondJit();
void FlushPc(int pc_offset = 0);
Status InitRecompiler();
void Invalidate(u32 addr);
void InvalidateRange(u32 addr_lo, u32 addr_hi);
void LinkJit(u32 reg);
void OnBranchNotTakenJit();
u32 RunRecompiler(u32 cpu_cycles);
void TearDownRecompiler();

inline JitCompiler c;
inline RegisterAllocator reg_alloc;
inline u64 jit_pc;
inline u32 block_cycles;
inline bool branch_hit;
inline bool branched;
inline bool compiler_exception_occurred;

template<typename T> asmjit::x86::Mem JitPtr(T const& obj, u32 ptr_size = sizeof(std::remove_pointer_t<T>))
{
    auto obj_ptr = [&] {
        if constexpr (std::is_pointer_v<T>) return obj;
        else return &obj;
    }();
    std::ptrdiff_t diff = reinterpret_cast<u8 const*>(obj_ptr) - reinterpret_cast<u8 const*>(&gpr);
    return asmjit::x86::ptr(asmjit::x86::rbp, s32(diff), ptr_size);
}

template<typename T>
asmjit::x86::Mem JitPtrOffset(T const& obj, u32 index, u32 ptr_size = sizeof(std::remove_pointer_t<T>))
{
    auto obj_ptr = [&] {
        if constexpr (std::is_pointer_v<T>) return obj;
        else return &obj;
    }();
    std::ptrdiff_t diff = reinterpret_cast<u8 const*>(obj_ptr) + index - reinterpret_cast<u8 const*>(&gpr);
    return asmjit::x86::ptr(asmjit::x86::rbp, s32(diff), ptr_size);
}

template<typename T>
asmjit::x86::Mem JitPtrOffset(T const& obj, asmjit::x86::Gp index, u32 ptr_size = sizeof(std::remove_pointer_t<T>))
{
    auto obj_ptr = [&] {
        if constexpr (std::is_pointer_v<T>) return obj;
        else return &obj;
    }();
    std::ptrdiff_t diff = reinterpret_cast<u8 const*>(obj_ptr) - reinterpret_cast<u8 const*>(&gpr);
    return asmjit::x86::ptr(asmjit::x86::rbp, index.r64(), 0u, s32(diff), ptr_size);
}

inline void TakeBranchJit(auto target)
{
    using namespace asmjit::x86;
    (void)target;
    // c.mov(JitPtr(in_branch_delay_slot_taken), 1);
    // c.mov(JitPtr(in_branch_delay_slot_not_taken), 0);
    // c.mov(JitPtr(branch_state), std::to_underlying(mips::BranchState::Perform));
    // if constexpr (std::integral<decltype(target)>) {
    //     c.mov(rax, target);
    //     c.mov(JitPtr(jump_addr), rax);
    // } else {
    //     c.mov(JitPtr(jump_addr), target.r64());
    // }
}

} // namespace ee
