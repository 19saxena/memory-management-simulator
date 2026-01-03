#pragma once
#include <cstdint>
#include <vector>

struct CacheLine {
    bool valid = false;
    uint64_t tag = 0;
    int freq = 0;          // ⭐ LFU counter
    uint64_t last_used = 0; // timestamp for tie-breaking if needed
};

enum class CachePolicy {
    FIFO,
    LRU,
    LFU  // ⭐ Add LFU option
};

class Cache {
public:
    Cache(size_t cache_size, size_t block_size, size_t assoc, CachePolicy policy);
    bool access(uint64_t address, uint64_t &time);
    void dump() const;
    void stats() const;

private:
    size_t size;
    size_t block;
    size_t associativity;
    size_t num_sets;
    CachePolicy replacement_policy;
    std::vector<std::vector<CacheLine>> sets;
};
