#include "mmu.hpp"
#include "cop0.hpp"
#include "ee.hpp"
#include "exceptions.hpp"
#include "frontend/message.hpp"
#include "timers.hpp"

#include <bit>
#include <cassert>
#include <cstring>
#include <format>

namespace ee {

template<EeUInt Int> static Int read_bios(u32 addr);
template<EeUInt Int> static Int read_io(u32 addr);
template<EeUInt Int> static Int read_rdram(u32 addr);
template<EeUInt Int> static void write_io(u32 addr, Int data);
template<MemOp> static u32 tlb_addr_translation(u32 vaddr);
template<MemOp> static u32 virt_to_phys_addr(u32 vaddr);

void TlbEntry::read() const
{
    cop0.entry_lo[0].raw = lo[0].raw;
    cop0.entry_lo[1].raw = lo[1].raw;
    cop0.entry_lo[0].g = cop0.entry_lo[1].g = hi.g;
    cop0.entry_hi.raw = hi.raw & ~page_mask & ~0x1F00;
    cop0.page_mask = page_mask;
}

void TlbEntry::write()
{
    lo[0].raw = cop0.entry_lo[0].raw & ~1;
    lo[1].raw = cop0.entry_lo[1].raw & ~1;
    hi.raw = cop0.entry_hi.raw & ~cop0.page_mask;
    hi.g = cop0.entry_lo[0].g & cop0.entry_lo[1].g;
    page_mask = cop0.page_mask;
    // Things that speed up virtual-to-physical-address translation
    vpn2_addr_mask = ~page_mask & ~0x1FFF;
    vpn2_compare = hi.raw & vpn2_addr_mask;
    offset_addr_mask = page_mask >> 1 | 0xFFF;
}

template<EeUInt Int> Int read_bios(u32 addr)
{
    static_assert(std::has_single_bit(bios.size()));
    Int ret;
    std::memcpy(&ret, &bios[addr & (bios.size() - 1)], sizeof(Int));
    return ret;
}

template<EeUInt Int> Int read_io(u32 addr)
{
    if constexpr (sizeof(Int) == 4) {
        if (addr < 0x1000'2000) {
            return timers::read(addr);
        }
        if (addr == 0x1000'F000) {
            return read_intc_stat();
        }
        if (addr == 0x1000'F010) {
            return read_intc_mask();
        }
    } else {
        message::fatal(std::format("Tried to read {} bytes from IO, when only word reads are supported.", sizeof(Int)));
        return {};
    }
}

template<EeUInt Int> Int read_rdram(u32 addr)
{
    static_assert(std::has_single_bit(rdram.size()));
    Int ret;
    std::memcpy(&ret, &rdram[addr & (rdram.size() - 1)], sizeof(Int));
    return ret;
}

template<MemOp mem_op> u32 tlb_addr_translation(u32 vaddr)
{
    for (TlbEntry const& entry : tlb_entries) {
        // Compare the virtual page number (divided by two; VPN2) of the entry with the VPN2 of the virtual address
        if ((vaddr & entry.vpn2_addr_mask) != entry.vpn2_compare) continue;
        // If the global bit is clear, the entry's ASID must coincide with the one in the EntryHi register
        if (!entry.hi.g && entry.hi.asid != cop0.entry_hi.asid) continue;
        // The VPN maps to two (consecutive) pages; EntryLo0 for even virtual pages and EntryLo1 for odd virtual pages.
        bool vpn_odd = vaddr & (entry.offset_addr_mask + 1);
        auto entry_lo = entry.lo[vpn_odd];
        if (!entry_lo.v) {
            tlb_invalid_exception(vaddr, mem_op);
            return 0;
        }
        if constexpr (mem_op == MemOp::DataWrite) {
            if (!entry_lo.d) {
                tlb_mod_exception(vaddr);
                return 0;
            }
        }
        // TLB hit
        u32 offset = vaddr & entry.offset_addr_mask;
        u32 pfn = entry_lo.pfn << 12 & ~entry.offset_addr_mask;
        return offset | pfn;
    }
    // TLB miss
    tlb_refill_exception(vaddr, mem_op);
    return 0;
}

template<MemOp mem_op> u32 virt_to_phys_addr(u32 vaddr)
{
    if (vaddr < 0x8000'0000) {
        return cop0.status.erl ? vaddr : tlb_addr_translation<mem_op>(vaddr);
    }
    if (vaddr < 0xC000'0000) {
        return vaddr & 0x1FFF'FFFF;
    }
    return tlb_addr_translation<mem_op>(vaddr);
}

template<EeUInt Int, Alignment alignment, MemOp mem_op> Int virtual_read(u32 addr)
{
    static constexpr size_t size = sizeof(Int);
    if constexpr (alignment == Alignment::Aligned && size > 1 && size < 16 && mem_op == MemOp::DataRead) {
        if (addr & (size - 1)) {
            address_error_exception(addr, mem_op);
            return {};
        }
    }

    u32 paddr = virt_to_phys_addr<mem_op>(addr);
    if (exception_occurred) return {};

    if (paddr < 0x0200'0000) return read_rdram<Int>(paddr);
    if (paddr < 0x1000'0000) assert(false);
    if (paddr < 0x1100'0000) return read_io<Int>(paddr);
    if (paddr < 0x1101'0000) {}
    if (paddr < 0x1200'0000) {}
    if (paddr < 0x1200'2000) {}
    if (paddr < 0x1C00'0000) assert(false);
    if (paddr < 0x2000'0000) return read_bios<Int>(paddr);
    assert(false);
    return {};
}

template<EeUInt Int> static void write_io(u32 addr, Int data)
{
    if constexpr (sizeof(Int) == 4) {
        if (addr < 0x1000'2000) {
            timers::write(addr, data);
        }
        if (addr == 0x1000'F000) {
            write_intc_stat(u16(data));
        }
        if (addr == 0x1000'F010) {
            write_intc_mask(u16(data));
        }
    } else {
        message::fatal(std::format("Tried to write {} bytes to IO, when only word writes are supported.", sizeof(Int)));
    }
}

template u8 virtual_read<u8, Alignment::Aligned, MemOp::DataRead>(u32);
template u16 virtual_read<u16, Alignment::Aligned, MemOp::DataRead>(u32);
template u32 virtual_read<u32, Alignment::Aligned, MemOp::DataRead>(u32);
template u64 virtual_read<u64, Alignment::Aligned, MemOp::DataRead>(u32);
template u128 virtual_read<u128, Alignment::Aligned, MemOp::DataRead>(u32);
template u32 virtual_read<u32, Alignment::Unaligned, MemOp::DataRead>(u32);
template u64 virtual_read<u64, Alignment::Unaligned, MemOp::DataRead>(u32);
template u32 virtual_read<u32, Alignment::Aligned, MemOp::InstrFetch>(u32);

} // namespace ee
