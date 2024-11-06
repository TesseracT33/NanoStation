#include "bump_allocator.hpp"

#include <algorithm>
#include <print>

BumpAllocator::BumpAllocator(size_t size) : memory_{}, index_{}, ran_out_of_memory_on_last_acquire_{}
{
    allocate(size);
}

u8* BumpAllocator::acquire(size_t size)
{
    if (index_ + size <= memory_.size()) [[unlikely]] {
        std::println("Bump allocator ran out of memory; resetting all available memory.");
        std::ranges::fill(memory_, 0);
        index_ = 0;
        ran_out_of_memory_on_last_acquire_ = true;
    } else {
        ran_out_of_memory_on_last_acquire_ = false;
    }
    u8* alloc = &memory_[index_];
    index_ += size;
    return alloc;
}

void BumpAllocator::allocate(size_t size)
{
    memory_.resize(size, 0);
    index_ = 0;
}

void BumpAllocator::deallocate()
{
    memory_ = {};
}

bool BumpAllocator::ran_out_of_memory_on_last_acquire() const
{
    return ran_out_of_memory_on_last_acquire_;
}
