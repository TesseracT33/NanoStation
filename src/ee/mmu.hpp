#pragma once

#include "numtypes.hpp"

#include <array>
#include <concepts>

namespace ee {

template<typename T>
concept ee_uint = std::unsigned_integral<T> || std::same_as<T, u128>;

enum class Alignment {
    Aligned,
    Unaligned
};

enum class MemOp {
    DataRead,
    DataWrite,
    InstrFetch
};

struct TlbEntry {
    union {
        u32 raw;
        struct {
            u32     : 1;
            u32 v   : 1;
            u32 d   : 1;
            u32 c   : 3;
            u32 pfn : 20;
            u32     : 5;
            u32 s   : 1; // scratchpad. applies only to lo1
        };
    } lo[2];

    union {
        u32 raw;
        struct {
            u32 asid : 8;
            u32      : 4;
            u32 g    : 1; // Global. If set in both LO0 and LO1, the ASID is ignored during TLB lookup.
            u32 vpn2 : 19;
        };
    } hi;

    u32 page_mask;

    u32 vpn2_addr_mask; // AND with vaddr => VPN2
    u32 vpn2_compare; // hi.vpn2 shifted left according to page_mask
    u32 offset_addr_mask; // AND with vaddr => offset

    void read() const;
    void write();
};

inline constexpr size_t bios_size = 4 * 1024 * 1024;

inline std::array<TlbEntry, 48> tlb_entries;

inline std::array<u8, bios_size> bios;
inline std::array<u8, 32 * 1024 * 1024> rdram;

u32 devirtualize(u32 vaddr);

template<ee_uint Int, Alignment alignment = Alignment::Aligned, MemOp mem_op = MemOp::DataRead>
Int virtual_read(u32 addr);

template<size_t size, Alignment alignment = Alignment::Aligned> void virtual_write(u32 addr, auto data);

template<size_t size, Alignment alignment> void virtual_write(u32 addr, auto data)
{
    static_assert(std::has_single_bit(size) && size <= 16 && size == sizeof(data));
    if constexpr (alignment == Alignment::Aligned && size > 1) {
        if (addr & (size - 1)) {
            // address_error_exception(addr, MemOp::DataWrite);
            return;
        }
    }
}

} // namespace ee
