#include "mmu.hpp"
#include "cop0.hpp"
#include "exceptions.hpp"

#include <bit>
#include <cstring>

namespace ee {

std::array<TlbEntry, 48> tlb_entries;

template<size_t size> static typename SizeToUInt<size>::type physical_read(u32 addr);
static void physical_write(u32 addr, auto data);
template<MemOp> static u32 tlb_addr_translation(u32 vaddr);
template<MemOp> static u32 virt_to_phys_addr(u32 vaddr);

void TlbEntry::read() const
{
    for (int i = 0; i < 2; ++i) {
        cop0.entry_lo[i].raw = lo[i].raw;
        cop0.entry_lo[i].g = hi.g;
    }
    cop0.entry_hi.raw = hi.raw & ~page_mask & ~0x1F00;
    cop0.page_mask = page_mask;
}

void TlbEntry::write()
{
    for (int i = 0; i < 2; ++i) {
        lo[i].raw = cop0.entry_lo[i].raw & ~1;
    }
    hi.raw = cop0.entry_hi.raw & ~cop0.page_mask;
    hi.g = cop0.entry_lo[0].g & cop0.entry_lo[1].g;
    page_mask = cop0.page_mask;
    // Things that speed up virtual-to-physical-address translation
    vpn2_addr_mask = ~page_mask & ~0x1FFF;
    vpn2_compare = hi.raw & vpn2_addr_mask;
    offset_addr_mask = page_mask >> 1 | 0xFFF;
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

template<size_t size, Alignment alignment, MemOp mem_op> typename SizeToUInt<size>::type virtual_read(u32 addr)
{
    static_assert(std::has_single_bit(size) && size <= 16);
    if constexpr (alignment == Alignment::Aligned && size > 1 && size < 16 && mem_op == MemOp::DataRead) {
        if (addr & (size - 1)) {
            address_error_exception(addr, mem_op);
            return {};
        }
    }

    u32 paddr = virt_to_phys_addr<mem_op>(addr);
    if (exception_occurred) return {};
    return {};
}

template u8 virtual_read<1, Alignment::Aligned, MemOp::DataRead>(u32);
template u16 virtual_read<2, Alignment::Aligned, MemOp::DataRead>(u32);
template u32 virtual_read<4, Alignment::Aligned, MemOp::DataRead>(u32);
template u64 virtual_read<8, Alignment::Aligned, MemOp::DataRead>(u32);
template u64x2 virtual_read<16, Alignment::Aligned, MemOp::DataRead>(u32);
template u32 virtual_read<4, Alignment::Unaligned, MemOp::DataRead>(u32);
template u64 virtual_read<8, Alignment::Unaligned, MemOp::DataRead>(u32);
template u32 virtual_read<4, Alignment::Aligned, MemOp::InstrFetch>(u32);

} // namespace ee
