#include "jit.hpp"
#include "asmjit/arm/a64compiler.h"
#include "asmjit/core/codeholder.h"
#include "asmjit/core/jitruntime.h"
#include "asmjit/x86/x86compiler.h"
#include "build_options.hpp"
#include "bump_allocator.hpp"
#include "cop0.hpp"
#include "ee.hpp"
#include "exceptions.hpp"
#include "jit_common.hpp"
#include "log.hpp"
#include "mips/decoder.hpp"
#include "mips/types.hpp"
#include "mmu.hpp"

#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <utility>
#include <vector>

using namespace asmjit;
using namespace asmjit::x86;

namespace ee {

constexpr u32 bytes_per_pool = 0x100;
constexpr u32 instructions_per_pool = bytes_per_pool / 4;
constexpr u32 num_pools = 0x80'0000; // 32 bits (address range) - 8 (bits per pool)
constexpr u32 pool_max_addr_excl = (num_pools * bytes_per_pool);
static_assert(std::has_single_bit(pool_max_addr_excl));

using Block = void (*)();

struct Pool {
    std::array<Block, instructions_per_pool> blocks;
};

static void compile(Block& block);
static void EmitInstruction();
static u32 FetchInstruction(u32 vaddr);
static void FinalizeBlock(Block& block);
static Block& GetBlock(u32 paddr);
static void PerformBranch();
static void ResetPool(Pool*& pool);
static void UpdateBranchState();

static BumpAllocator allocator;
static asmjit::CodeHolder code_holder;
static asmjit::FileLogger jit_logger(stdout);
static asmjit::JitRuntime jit_runtime;
static std::vector<Pool*> pools;
static bool block_has_branch_instr;

void BlockEpilog()
{
    RecordBlockCycles();
    reg_alloc.BlockEpilog();
    c.ret();
}

void BlockEpilogWithJmp(void (*func)())
{
    RecordBlockCycles();
    reg_alloc.BlockEpilogWithJmp(func);
}

void BlockEpilogWithPcFlushAndJmp(void (*func)(), int pc_offset)
{
    FlushPc(pc_offset);
    BlockEpilogWithJmp(func);
}

void BlockEpilogWithPcFlush(int pc_offset)
{
    FlushPc(pc_offset);
    BlockEpilog();
}

void BlockProlog()
{
    code_holder.reset();
    asmjit::Error err = code_holder.init(jit_runtime.environment(), jit_runtime.cpuFeatures());
    if (err) [[unlikely]] {
        log_fatal("Failed to init asmjit code holder; returned {}", asmjit::DebugUtils::errorAsString(err));
    }
    err = code_holder.attach(&c);
    if (err) [[unlikely]] {
        log_fatal("Failed to attach asmjit compiler to code holder; returned {}",
          asmjit::DebugUtils::errorAsString(err));
    }
    if constexpr (enable_ee_jit_error_handler) {
        static AsmjitLogErrorHandler asmjit_log_error_handler{};
        code_holder.setErrorHandler(&asmjit_log_error_handler);
    }
    if constexpr (log_ee_jit_blocks) {
        jit_logger.addFlags(FormatFlags::kMachineCode);
        code_holder.setLogger(&jit_logger);
        jit_logger.log("======== CPU BLOCK BEGIN ========\n");
    }
    c.addFunc(FuncSignature::build<void>());
    reg_alloc.BlockProlog();
}

bool CheckDwordOpCondJit()
{
    // if (can_execute_dword_instrs) {
    //     return true;
    // } else {
    //     BlockEpilogWithPcFlushAndJmp(reserved_instruction_exception);
    //     branched = true;
    //     return false;
    // }
    return true;
}

void compile(Block& block)
{
    branched = block_has_branch_instr = false;
    block_cycles = 0;
    jit_pc = pc;

    BlockProlog();

    EmitInstruction();

    if (compiler_exception_occurred) {
        BlockEpilog();
        FinalizeBlock(block);
        return;
    }

    // If the previously executed block ended with a branch instruction, meaning that the branch delay
    // slot did not fit, execute only the first instruction in this block, before jumping.
    // The jump can be cancelled if the first instruction is also a branch.
    if (!branch_hit) {
        UpdateBranchState();
    }

    while (!branched && !compiler_exception_occurred && (jit_pc & 255)) {
        branched |= branch_hit; // If the branch delay slot instruction fits within the block boundary,
                                // include it before stopping
        EmitInstruction();
    }

    if (compiler_exception_occurred) {
        BlockEpilog();
    } else {
        if (!branch_hit && block_has_branch_instr) {
            UpdateBranchState();
        }
        BlockEpilogWithPcFlush(0);
    }

    FinalizeBlock(block);
}

void DiscardBranch()
{
    c.mov(JitPtr(in_branch_delay_slot_taken), 0);
    c.mov(JitPtr(in_branch_delay_slot_not_taken), 0);
    c.mov(JitPtr(branch_state), std::to_underlying(mips::BranchState::NoBranch));
    BlockEpilogWithPcFlush(8);
}

void EmitInstruction()
{
    block_cycles++;
    branch_hit = compiler_exception_occurred = false;
    u32 instr = FetchInstruction(jit_pc);
    if (compiler_exception_occurred) {
        return; // todo: handle this. need to compile exception handling
    }
    mips::decode_ee(instr);
    if (compiler_exception_occurred) {
        return;
    }
    jit_pc += 4;
    block_has_branch_instr |= branch_hit;
    if constexpr (log_ee_jit_register_status) {
        jit_logger.log(reg_alloc.GetStatus().c_str());
    }
}

void EmitLink(u32 reg)
{
    c.mov(reg_alloc.GetDirtyGpr(reg), jit_pc + 8);
}

u32 FetchInstruction(u32 vaddr)
{
    return virtual_read<u32, Alignment::Aligned, MemOp::InstrFetch>(vaddr);
}

void FinalizeBlock(Block& block)
{
    c.endFunc();
    asmjit::Error err = c.finalize();
    if (err) {
        log_fatal("Failed to finalize code block; returned {}", asmjit::DebugUtils::errorAsString(err));
    }
    err = jit_runtime.add(&block, &code_holder);
    if (err) {
        log_fatal("Failed to add code to asmjit runtime! Returned error code {}.", err);
    }
}

void FlushPc(int pc_offset)
{
    u32 new_pc = jit_pc + pc_offset;
    if (new_pc - pc < 128) {
        c.add(JitPtr(pc), new_pc);
    } else {
        c.mov(JitPtr(pc), new_pc);
    }
}

Block& GetBlock(u32 paddr)
{
    static_assert(std::has_single_bit(num_pools));
    Pool*& pool = pools[paddr >> 8 & (num_pools - 1)]; // each pool 6 bits, each instruction 2 bits
    if (!pool) {
        pool = allocator.acquire<Pool>(); // TODO: check if OOM
    }
    assert(pool);
    return pool->blocks[paddr >> 2 & 63];
}

Status InitJit()
{
    allocator.allocate(64_MiB);
    pools.resize(num_pools, nullptr);
    return OkStatus();
}

void Invalidate(u32 paddr)
{
    assert(paddr < pool_max_addr_excl);
    Pool*& pool = pools[paddr >> 8 & (num_pools - 1)]; // each pool 6 bits, each instruction 2 bits
    ResetPool(pool);
}

void InvalidateRange(u32 paddr_lo, u32 paddr_hi)
{
    assert(paddr_lo <= paddr_hi);
    assert(paddr_hi < pool_max_addr_excl);
    u32 pool_lo = paddr_lo >> 8;
    u32 pool_hi = paddr_hi >> 8;
    std::for_each(pools.begin() + pool_lo, pools.begin() + pool_hi + 1, [](Pool*& pool) { ResetPool(pool); });
}

void OnBranchNotTaken()
{
    c.mov(JitPtr(in_branch_delay_slot_taken), 0);
    c.mov(JitPtr(in_branch_delay_slot_not_taken), 1);
    c.mov(JitPtr(branch_state), std::to_underlying(mips::BranchState::NoBranch));
}

void PerformBranch()
{
    in_branch_delay_slot_taken = false;
    branch_state = mips::BranchState::NoBranch;
    pc = jump_addr;
    if (pc & 3) {
        address_error_exception(pc, MemOp::InstrFetch);
    }
    if constexpr (log_ee_branches) {
        log_info("EE branch to 0x{:016X}; RA = 0x{:016X}; SP = 0x{:016X}", u64(pc), u64(gpr[31]), u64(gpr[29]));
    }
}

void RecordBlockCycles()
{
    assert(block_cycles > 0);
    c.add(JitPtr(cycle_counter), block_cycles);
    c.add(JitPtr(cop0.count), block_cycles);
}

void ResetPool(Pool*& pool)
{
    if (pool) {
        for (Block block : pool->blocks) {
            if (block) {
                jit_runtime.release(block);
            }
        }
        pool = nullptr;
    }
}

u32 RunJit(u32 cycles)
{
    cycle_counter = 0;
    while (cycle_counter < cycles) {
        exception_occurred = false;
        Block& block = GetBlock(devirtualize(pc));
        if (!block) {
            compile(block);
        }
        block();
    }
    return cycle_counter;
}

template<typename Target>
void TakeBranch(Target target)
    requires(std::same_as<Target, u32> || std::same_as<Target, HostGpr32>)
{
    c.mov(JitPtr(in_branch_delay_slot_taken), 1);
    c.mov(JitPtr(in_branch_delay_slot_not_taken), 0);
    c.mov(JitPtr(branch_state), std::to_underlying(mips::BranchState::Perform));
    c.mov(JitPtr(jump_addr), target);
}

void TearDownJit()
{
    allocator.deallocate();
    pools.clear();
}

void UpdateBranchState()
{
    Label l_nobranch = c.newLabel();
    c.cmp(JitPtr(branch_state), std::to_underlying(mips::BranchState::Perform));
    c.jne(l_nobranch);
    BlockEpilogWithJmp(PerformBranch);
    c.bind(l_nobranch);
    c.mov(JitPtr(in_branch_delay_slot_not_taken), 0);
}

template void TakeBranch<u32>(u32);
template void TakeBranch<HostGpr32>(HostGpr32);

} // namespace ee
