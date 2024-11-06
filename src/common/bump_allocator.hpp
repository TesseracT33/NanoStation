#pragma once

#include "numtypes.hpp"

#include <vector>

class BumpAllocator {
public:
    BumpAllocator(size_t size = 0);

    u8* acquire(size_t size);
    void allocate(size_t size);
    void deallocate();
    bool ran_out_of_memory_on_last_acquire() const;

private:
    std::vector<u8> memory_;
    size_t index_;
    bool ran_out_of_memory_on_last_acquire_;
};