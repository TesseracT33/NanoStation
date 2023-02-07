#pragma once

#include "mips/mips.hpp"
#include "types.hpp"

namespace iop {
template<mips::CpuImpl> void mfc0(u32 rd, u32 rt);
template<mips::CpuImpl> void mtc0(u32 rd, u32 rt);
template<mips::CpuImpl> void rfe();

struct Cop0Registers {
    u32 unused_0, unused_1, unused_2;

    u32 bpc;

    u32 unused_4;

    u32 bda;

    u32 jumpdest;

    struct {
        u32 data; // TODO
    } dcic;

    u32 bad_v_addr;

    u32 bdam;

    u32 unusued_10;

    u32 bpcm;

    union {
        struct {
            u32 iec : 1;
            u32 kuc : 1;
            u32 iep : 1;
            u32 kup : 1;
            u32 ieo : 1;
            u32 kuo : 1;
            u32     : 2;
            u32 im  : 8;
            u32 isc : 1;
            u32 swc : 1;
            u32 pz  : 1;
            u32 cm  : 1;
            u32 pe  : 1;
            u32 ts  : 1;
            u32 bev : 1;
            u32     : 5;
            u32 cu0 : 1;
            u32 cu1 : 1;
            u32 cu2 : 1;
            u32 cu3 : 1;
        };
        u32 raw;
    } status;

    union {
        struct {
            u32        : 2;
            u32 excode : 5;
            u32        : 1;
            u32 ip     : 8;
            u32        : 12;
            u32 ce     : 2;
            u32        : 1;
            u32 bd     : 1;
        };
        u32 raw;
    } cause;

    u32 epc;

    struct {
        u32 rev : 8 = 2; /* Processor revision number */
        u32 imp : 8 = 3; /* Processor ID number */
        u32     : 16;
    } const pr_id;
} inline cop0;

} // namespace iop
