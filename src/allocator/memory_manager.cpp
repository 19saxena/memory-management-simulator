#include "memory_manager.h"
#include <iterator>
#include <iostream>
#include <limits>
#include<algorithm>

MemoryManagerSimulator::MemoryManagerSimulator()
   : allocator(AllocatorType::FIRST_FIT), memory_size(0), alloc_success(0), alloc_fail(0), internal_frag(0), time(0), next_id(1) {}

void MemoryManagerSimulator::init(size_t total_size) {
    memory_size = total_size;
    alloc_success = 0;
    alloc_fail = 0;
    internal_frag = 0;
    time = 0;
    next_id = 1;
    blocks.clear();

    Block b;
    b.start = 0;
    b.size = total_size;
    b.free = true;
    b.id = -1;
    b.requested = 0;
    b.internal_frag = 0;

    blocks.push_back(b);
}


void MemoryManagerSimulator::setAllocator(AllocatorType type) {
    allocator = type;
}

std::list<Block>::iterator MemoryManagerSimulator::find_first_fit(size_t size) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it)
        if (it->free && it->size >= size)
            return it;
    return blocks.end();
}

std::list<Block>::iterator MemoryManagerSimulator::find_best_fit(size_t size) {
    auto best = blocks.end();
    size_t smallest = std::numeric_limits<size_t>::max();
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->free && it->size >= size && it->size < smallest) {
            smallest = it->size;
            best = it;
        }
    }
    return best;
}

std::list<Block>::iterator MemoryManagerSimulator::find_worst_fit(size_t size) {
    auto worst = blocks.end();
    size_t largest = 0;
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (it->free && it->size >= size && it->size > largest) {
            largest = it->size;
            worst = it;
        }
    }
    return worst;
}

int MemoryManagerSimulator::malloc_block(size_t size) {
    const size_t ALIGN = 16;
    size_t actual = (size + ALIGN - 1) & ~(ALIGN - 1);

    auto it = blocks.end();
    if (allocator == AllocatorType::FIRST_FIT)
        it = find_first_fit(actual);
    else if (allocator == AllocatorType::BEST_FIT)
        it = find_best_fit(actual);
    else if (allocator == AllocatorType::WORST_FIT)
        it = find_worst_fit(actual);

    if (it == blocks.end()) {
        alloc_fail++;
        return -1;
    }

    alloc_success++;
    int id = next_id++;  // ⭐ FIXED

    if (it->size > actual) {
        Block rem;
        rem.start = it->start + actual;
        rem.size = it->size - actual;
        rem.free = true;
        rem.id = -1;
        blocks.insert(std::next(it), rem);
    }

    it->size = actual;
    it->free = false;
    it->id = id;
    it->requested = size;
    it->internal_frag = actual - size;
    internal_frag += (actual - size);

    return id;
}


bool MemoryManagerSimulator::free_block(int block_id) {
    for (auto it = blocks.begin(); it != blocks.end(); ++it) {
        if (!it->free && it->id == block_id) {
            it->free = true;
            it->id = -1;

            auto nx = std::next(it);
            if (nx != blocks.end() && nx->free) {
                it->size += nx->size;
                blocks.erase(nx);
            }

            if (it != blocks.begin()) {
                auto pv = std::prev(it);
                if (pv->free) {
                    pv->size += it->size;
                    blocks.erase(it);
                }
            }
            return true;
        }
    }
    return false;
}

void MemoryManagerSimulator::dump() const {
    std::cout << "\n=== MEMORY DUMP ===\n";
    for (const auto &b : blocks) {
        std::cout << "[0x" << std::hex << b.start << " - 0x"
                  << (b.start + b.size - 1) << "] "
                  << (b.free ? "FREE" : "USED");
        if (!b.free) std::cout << " (id=" << b.id << ")";
        std::cout << "\n";
    }
    std::cout << std::dec;
}
void MemoryManagerSimulator::stats() const {
    size_t used = 0;
    size_t largest_free = 0;
    size_t free_mem = 0;

    for (const auto &b : blocks) {
        if (b.free) {
            free_mem += b.size;
            largest_free = std::max(largest_free, b.size);
        } else {
            used += b.size;
        }
    }

    double utilization = memory_size ? ((double)used / memory_size) * 100.0 : 0.0;
    double ext_frag = free_mem ? (1.0 - (double)largest_free / free_mem) * 100.0 : 0.0;
    int total_requests = alloc_success + alloc_fail;
    double success_rate = total_requests ? ((double)alloc_success / total_requests) * 100.0 : 0.0;
    double failure_rate = 100.0 - success_rate;

    std::cout << "\n=== MEMORY STATS ===\n";
    std::cout << "Total memory: " << memory_size << " bytes\n";
    std::cout << "Used memory: " << used << " bytes\n";
    std::cout << "Free memory: " << (memory_size - used) << " bytes\n";
    std::cout << "Memory utilization: " << utilization << "%\n";
    std::cout << "Internal fragmentation: " << internal_frag << " bytes\n";
    std::cout << "External fragmentation: " << ext_frag << "%\n";
    std::cout << "Allocation success rate: " << success_rate << "%\n";
    std::cout << "Allocation failure rate: " << failure_rate << "%\n";
    std::cout << "Total allocation requests: " << total_requests << "\n";
}


void MemoryManagerSimulator::visualize() const {
    size_t used_mem = 0;
    for (const auto &b : blocks)
        if (!b.free) used_mem += b.size;

    int scale = 50;
    size_t total_units = memory_size;

    std::string bar;
    for (int i = 0; i < scale; i++) {
        size_t addr = (size_t)((double)i / scale * total_units);
        auto it = std::find_if(blocks.begin(), blocks.end(), [&](const Block &b) {
            return !b.free && addr >= b.start && addr < b.start + b.size;
        });
        bar += (it != blocks.end()) ? "#" : "_";
    }

    std::cout << "\n=== MEMORY VISUALIZATION ===\n";
    std::cout << "[" << bar << "]\n";
    std::cout << "_ = FREE, # = USED\n";
}
