#pragma once
#include "cache.h"
#include <cstdint>

class MultiLevelCache {
public:
    MultiLevelCache(Cache l1, Cache l2);
    bool access(uint64_t address);
    void dump() const;
    void stats() const;

private:
    Cache L1, L2;
    uint64_t time = 0;
    int l1_hits = 0, l1_misses = 0;
    int l2_hits = 0, l2_misses = 0;
};
