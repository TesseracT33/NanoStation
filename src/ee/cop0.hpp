#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

namespace ee {
template<mips::CpuImpl> void bc0f(s16 imm);
template<mips::CpuImpl> void bc0fl(s16 imm);
template<mips::CpuImpl> void bc0t(s16 imm);
template<mips::CpuImpl> void bc0tl(s16 imm);
template<mips::CpuImpl> void di();
template<mips::CpuImpl> void ei();
template<mips::CpuImpl> void eret();
template<mips::CpuImpl> void mfc0(u32 rd, u32 rt);
template<mips::CpuImpl> void mtc0(u32 rd, u32 rt);
template<mips::CpuImpl> void tlbp();
template<mips::CpuImpl> void tlbr();
template<mips::CpuImpl> void tlbwi();
template<mips::CpuImpl> void tlbwr();

template<bool initial_add = false> void reload_count_compare_event();

struct Cop0Registers {
    struct { /* (0) */
        u32 value : 6; /* Index to the TLB entry affected by the TLB Read (TLBR) and TLB Write (TLBW) instructions. */
        u32       : 25;
        u32 p     : 1; /* Shows the success (0) or failure (1) of the last TLB Probe (TLBP) instruction executed. */
    } index;

    u32 random; /* (1) 6 bits; Decremented every instruction, and specifies the entry in the TLB that is affected by the
                   TLB Write instruction. */

    union { /* (2), (3); Used to rewrite the TLB or to check coincidence of a TLB entry when addresses are
                        converted. */
        struct {
            u32 g : 1; /* Global. If this bit is set in both EntryLo0 and EntryLo1, then the processor ignores the ASID
              during TLB lookup. */
            u32 v : 1; /* Valid. If this bit is set, it indicates that the TLB entry is valid; otherwise, a TLBL or TLBS
                          miss occurs. */
            u32 d : 1; /* Dirty. If this bit is set, the page is marked as dirty, and therefore writable. */
            u32 c : 3; /* Specifies the TLB page attribute. */
            u32 pfn : 20; /* Page frame number -- the high-order bits of the physical address. */
            u32     : 5;
            u32 s   : 1; // Scratchpad. When set, the virtual mapping goes to scratchpad instead of main memory. Only
                       // applies to entryLo0
        };
        u32 raw;
    } entry_lo[2];

    struct { /* (4) */
        u32          : 4;
        u32 bad_vpn2 : 19; /* The virtual page number divided by two, given during e.g. a TLB miss.  */
        u32 pte_base : 9; /* The base address of the page table entry, given during e.g. a TLB miss.  */
    } context;

    u32 page_mask; /* (5) Sets the page size for each TLB entry. Bits 0-12, 25-31 are zero. The rest:
        0 => 4 KB; 3 => 16 KB; 15 => 64 KB; 63 => 256 KB; 255 => 1 MB; 1023 => 4 MB; 4095 => 16 MB.
        Else, the operation of the TLB is undefined. As 0 represent 4 KB i.e. 12 bits, and bit 12 is 0,
        this register is actually used to mask a virtual addr to get its VPN2. */

    u32 wired; /* (6); 6 bits; Specifies the boundary between the "wired" and "random" entries of the TLB; wired entries
                  cannot be overwritten by a TLBWR operation. */

    u32 unused_7; /* (7) */

    u32 bad_v_addr; /* (8) Virtual address at which an address error occurred last or which failed in address
                       translation. */

    u32 count; /* (9); Increments every other PClock. When equal to the Compare register, interrupt bit IP(7) in the
                  Cause register is set. */

    union { /* (10) */
        struct {
            u32 asid : 8; /* Address space ID field. Lets multiple processes share the TLB; virtual addresses for each
                 process can be shared. */
            u32      : 5;
            u32 vpn2 : 19; /* Virtual page number divided by two (maps to two pages). */
        };
        u32 raw;
    } entry_hi;

    u32 compare; /* (11); When equal to the Count register, interrupt bit IP(7) in the Cause register is set. Writes to
                    this register clear said interrupt. */
    /* On real HW, this register is 32 bits. Here, we make it 64 bits. See the description of the 'Count' register. */

    struct { /* (12) */
        u32 ie  : 1; /* Interrupt Enable (0: disable interrupts; 1: enable interrupts (if EIEI=1, ERL=0, EXL=0)) */
        u32 exl : 1; /* Exception Level (0: normal; 1: exception) */
        u32 erl : 1; /* Error level (0: normal; 1: error) */
        u32 ksu : 2; /* Specifies and indicates mode bits (00: kernel; 01: supervisor; 10: user; 11: reserved) */
        u32     : 5;
        u32 im2 : 1; // Interrupt Mask bit 2 (INTC) (0: disabled; 1: enabled)
        u32 im3 : 1; // Interrupt Mask bit 3 (DMAC) (0: disabled; 1: enabled)
        u32 bem : 1;
        u32     : 2;
        u32 im7 : 1; // Interrupt Mask bit 7 (COP0 timer) (0: disabled; 1: enabled)
        u32 eie : 1; // Enable IE (0: disabled; 1: enabled)
        u32 edi : 1; // EI/DI instruction Enable in supervisor/user modes (0: disabled; 1: enabled)
        u32 ch  : 1; /* Cache Hit (0: miss; 1: hit) */
        u32     : 3;
        u32 bev : 1; /* Controls the location of TLB refill and general exception vectors (0: normal; 1:
                        bootstrap) */
        u32 dev : 1; /* Controls the location of Performance counter and debug/SIO exception vectors (0: normal; 1:
                        bootstrap) */
        u32     : 2;
        u32 fr  : 1; /* Enable additional floating-point registers (0: 16 registers; 1: 32 registers) */
        u32     : 1;
        u32 cu0 : 1; /* COP0 is usable. If cleared, COP0 instructions signal exceptions, unless in kernel mode */
        u32 cu1 : 1; /* COP1 is usable. If cleared, COP1 instructions signal exceptions */
        u32 cu2 : 1; /* COP2 is usable. If cleared, COP2 instructions signal exceptions */
        u32 cu3 : 1; /* COP3 is usable. If cleared, COP3 instructions signal exceptions */
    } status;

    struct { /* (13) */
        u32          : 2;
        u32 exc_code : 5; /* Exception code field; written to when an exception is signalled. */
        u32          : 3;
        u32 ip2      : 1; // Indicates that an interrupt is pending, bit 2 (INTC) (0: no interrupt; 1: interrupt)
        u32 ip3      : 1; // Indicates that an interrupt is pending, bit 3 (DMAC) (0: no interrupt; 1: interrupt)
        u32 siop     : 1;
        u32          : 2;
        u32 ip7      : 1; // Indicates that an interrupt is pending, bit 7 (COP0 timer) (0: no interrupt; 1: interrupt)
        u32 exc2 : 3; //  Indicates the exception codes for level 2 exceptions (Performance Counter, Reset, Debug, SIO
                      //  and NMI exceptions) (0: Reset; 1: NMI; 2: PerfC; 3: Debug and SIO; 4-7: reserved)
        u32      : 9;
        u32 ce   : 2; /* Coprocessor unit number referenced when a Coprocessor Unusable exception has occurred. */
        u32 bd2 : 1; /* Indicates whether the last NMI, performance counter, debug, or SIO exception taken occurred in a
                        branch delay slot (0: normal; 1: delay slot) */
        u32 bd  : 1; //  Set by the processor when any exception other than Reset, NMI, performance counter, or debug
                    //  occurs and is taken in a branch delay slot (0: normal; 1: delay slot)
    } cause;

    u32 epc; /* (14) Contains the address at which processing resumes after an exception has been serviced. */

    struct { /* (15) */
        u32 rev : 8 = 0x00; /* Processor revision number */
        u32 imp : 8 = 0x38; /* Processor ID number */
        u32     : 16;
    } const pr_id;

    struct { /* (16) */
        u32 k0  : 3; /* Sets coherency algorithm of kseg0 (010 => cache is not used; else => cache is used). */
        u32     : 3;
        u32 dc  : 3;
        u32 ic  : 3;
        u32 bpe : 1;
        u32 nbe : 1;
        u32     : 1;
        u32 be  : 1; /* Sets endianness (0 => little endian; 1 => big endian (default on cold reset). */
        u32 dce : 1;
        u32 ice : 1;
        u32 die : 1;
        u32     : 9;
        u32 ec  : 3; /* Operating frequency ratio (read-only). */
        u32     : 1;
    } config;

    u32 unused_17, unused_18, unused_19, unused_20, unused_21, unused_22;

    struct { // (23)
        u32       : 4;
        u32 value : 28;
    } bad_p_addr;

    struct { // (24)
        u32 iab : 1;
        u32 drb : 1;
        u32 dwb : 1;
        u32     : 12;
        u32 bed : 1;
        u32 dte : 1;
        u32 ite : 1;
        u32 dxe : 1;
        u32 dke : 1;
        u32 dse : 1;
        u32 due : 1;
        u32     : 1;
        u32 ie  : 1;
        u32 ike : 1;
        u32 ise : 1;
        u32 iue : 1;
        u32     : 1;
        u32 dve : 1;
        u32 dwe : 1;
        u32 dre : 1;
        u32 iae : 1;
    } debug;

    struct { // (25; PCCR)
        u32        : 1;
        u32 exl0   : 1;
        u32 k0     : 1;
        u32 s0     : 1;
        u32 u0     : 1;
        u32 event0 : 5;
        u32        : 1;
        u32 exl1   : 1;
        u32 k1     : 1;
        u32 s1     : 1;
        u32 u1     : 1;
        u32 event1 : 5;
        u32        : 11;
        u32 cte    : 1;
    } perf_pccr;

    u32 unused_26, unused_27;

    struct { /* (28) */
        u32 su   : 3;
        u32 l    : 1;
        u32 r    : 1;
        u32 v    : 1;
        u32 d    : 1;
        u32      : 5;
        u32 ptag : 20;
    } tag_lo; /* Holds the primary cache tag for cache initialization, cache diagnostics, or cache error processing. The
                 Tag registers are written by the CACHE and MTC0 instructions. */

    u32 tag_hi; /* (29); */

    u32 error_epc; /* (30) */

    u32 unused_31;

    struct { // (24)
        u32       : 2;
        u32 value : 30;
    } debug_iab, debug_iabm;

    u32 debug_dab, debug_dabm, debug_dvb, debug_dvbm;

    struct { // (25; PCR0, PCR1)
        u32 value : 31;
        u32 ovfl  : 1;
    } perf_pcr0, perf_pcr1;

    u32 get(int reg);
    template<bool raw = false> void set(int reg, u32 value);
    void set_raw(int reg, u32 value) { set<true>(reg, value); }
    void on_write_to_cause();
    void on_write_to_compare();
    void on_write_to_count();
    void on_write_to_status();
    void update_random();
} extern cop0;

extern u64 cycles_since_updated_random;

} // namespace ee
