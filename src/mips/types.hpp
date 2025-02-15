#pragma once

namespace mips {

enum class BranchState {
    DelaySlotNotTaken,
    DelaySlotTaken,
    NoBranch,
    Perform,
};

enum class Cond {
    Eq,
    Ge,
    Geu,
    Gt,
    Le,
    Lt,
    Ltu,
    Ne,
};

enum class OperatingMode {
    User,
    Supervisor,
    Kernel,
};

} // namespace mips
