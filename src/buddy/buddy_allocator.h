#pragma once
#include <cstddef>
#include <list>
#include <map>
#include <cstdint>
#include <cmath>
#include <iostream>

struct BuddyBlock {
    uint64_t start;
    uint64_t size;
    int id;
    BuddyBlock(uint64_t s, uint64_t sz, int i) : start(s), size(sz), id(i) {}
    BuddyBlock() = default;
};

class BuddyAllocator {
public:
    BuddyAllocator(size_t total_size);
    int allocate(uint64_t request_size);
    bool freeBlock(int block_id);
    void dump() const;
    void stats() const;

private:
    size_t memory_size;
    size_t total_memory;      // ⭐ FIXED: add this
    uint64_t next_id = 1;     // block counter
    int alloc_success = 0;
    int alloc_fail = 0;
    std::map<size_t, std::list<BuddyBlock>> free_lists;
    std::map<int, BuddyBlock> allocated_blocks;

    uint64_t nextPowerOf2(uint64_t n);
    uint64_t getBuddy(uint64_t addr, uint64_t size);
};
