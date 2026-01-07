#include "buddy_allocator.h"

BuddyAllocator::BuddyAllocator(size_t total_size)
    : memory_size(total_size), total_memory(total_size), alloc_success(0), alloc_fail(0)
{
    size_t power = 1;
    while (power < total_size) power *= 2;
    if (power != total_size) {
        std::cout << "Buddy memory must be a power of 2! Rounding up to " << power << "\n";
        memory_size = power;
        total_memory = power;
    }
    free_lists[memory_size].push_back(BuddyBlock(0, memory_size, -1));
}


uint64_t BuddyAllocator::nextPowerOf2(uint64_t n) {
    if (n == 0) return 1;
    n--;
    n |= n >> 1;
    n |= n >> 2;
    n |= n >> 4;
    n |= n >> 8;
    n |= n >> 16;
    n |= n >> 32;
    return n + 1;
}


uint64_t BuddyAllocator::getBuddy(uint64_t addr, uint64_t size) {
    return addr ^ size;
}

int BuddyAllocator::allocate(uint64_t request_size) {
    uint64_t actual_size = nextPowerOf2(request_size);
    uint64_t curr_size = actual_size;

    while (curr_size <= memory_size && free_lists[curr_size].empty()) {
        curr_size *= 2;
    }

    if (curr_size > memory_size || free_lists[curr_size].empty()) {
        alloc_fail++;
        std::cout << "Buddy allocation failed\n";
        return -1;
    }

    alloc_success++;
    auto block = free_lists[curr_size].front();
    free_lists[curr_size].pop_front();
    while (curr_size > actual_size) {
        curr_size /= 2;
        uint64_t buddy_addr = getBuddy(block.start, curr_size);
        free_lists[curr_size].push_back(BuddyBlock(buddy_addr, curr_size, -1));
    }

    int id = next_id++;
    allocated_blocks[id] = BuddyBlock(block.start, curr_size, id);

    std::cout << "Allocated buddy block id=" << id
              << " at address 0x" << std::hex << block.start << std::dec << "\n";

    return id;
}


bool BuddyAllocator::freeBlock(int block_id) {
    if (!allocated_blocks.count(block_id)) {
        std::cout << "Invalid buddy block ID\n";
        return false;
    }

    auto block = allocated_blocks[block_id];
    allocated_blocks.erase(block_id);

    while (true) {
        uint64_t buddy_addr = getBuddy(block.start, block.size);
        bool merged = false;

        for (auto it = free_lists[block.size].begin(); it != free_lists[block.size].end(); ++it) {
            if (it->start == buddy_addr) {
                block.start = std::min(block.start, buddy_addr);
                free_lists[block.size].erase(it);
                block.size *= 2;
                merged = true;
                break;
            }
        }

        if (!merged) break;
    }

    free_lists[block.size].push_back(BuddyBlock(block.start, block.size, -1));
    std::cout << "Buddy block " << block_id << " freed and merged\n";
    return true;
}


void BuddyAllocator::dump() const {
    std::cout << "\n=== BUDDY MEMORY DUMP ===\n";
    for (const auto &p : free_lists)
        for (const auto &blk : p.second)
            std::cout << "[0x" << std::hex << blk.start
                      << " - 0x" << (blk.start + blk.size - 1)
                      << "] FREE\n";
    std::cout << std::dec;
}

void BuddyAllocator::stats() const {
    size_t used = 0;
    for (const auto &b : allocated_blocks) {
        used += b.second.size;
    }

    size_t free_mem = memory_size - used;
    double utilization = (double)used / memory_size * 100.0;

    std::cout << "\n=== BUDDY ALLOCATOR STATS ===\n";
    std::cout << "Total memory: " << memory_size << " bytes\n";
    std::cout << "Used memory: " << used << " bytes\n";
    std::cout << "Free memory: " << free_mem << " bytes\n";
    std::cout << "Memory utilization: " << std::fixed << utilization << "%\n";
    std::cout << "Total allocation requests: " << (alloc_success + alloc_fail) << "\n";
}
