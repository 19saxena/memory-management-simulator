#include "multilevel_cache.h"
#include <iostream>

MultiLevelCache::MultiLevelCache(Cache l1, Cache l2)
    : L1(l1), L2(l2) {}

bool MultiLevelCache::access(uint64_t address) {
    if (L1.access(address, time)) {
        l1_hits++;
        return true;
    }

    l1_misses++;
    if (L2.access(address, time)) {
        l2_hits++;
        return true;
    }

    l2_misses++;
    time++;
    return false;
}

void MultiLevelCache::dump() const {
    std::cout << "\n=== MULTILEVEL CACHE DUMP ===\n";
    L1.dump();
    L2.dump();
}

void MultiLevelCache::stats() const {
    int total_L1 = l1_hits + l1_misses;
    int total_L2 = l2_hits + l2_misses;

    std::cout << "\n=== MULTILEVEL CACHE STATS ===\n";
    std::cout << "L1 hits: " << l1_hits << "  L1 misses: " << l1_misses << "\n";
    std::cout << "L2 hits: " << l2_hits << "  L2 misses: " << l2_misses << "\n";

    std::cout << "L1 hit ratio: " << (total_L1 ? (double)l1_hits/total_L1*100.0 : 0.0) << "%\n";
    std::cout << "L2 hit ratio: " << (total_L2 ? (double)l2_hits/total_L2*100.0 : 0.0) << "%\n";
}
