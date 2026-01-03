#include "cache.h"
#include <iostream>
#include <algorithm>

Cache::Cache(size_t cache_size, size_t block_size, size_t assoc, CachePolicy policy)
    : size(cache_size), block(block_size), associativity(assoc), replacement_policy(policy) {

    num_sets = size / (block * associativity);
    if (num_sets == 0) num_sets = 1;
    sets.resize(num_sets, std::vector<CacheLine>(associativity));
}

bool Cache::access(uint64_t address, uint64_t &time) {
    uint64_t tag = address / block;
    uint64_t set = tag % num_sets;

    // Check for HIT
    for (auto &line : sets[set]) {
        if (line.valid && line.tag == tag) {
            line.freq++;      // ⭐ increase LFU count on hit
            line.last_used = time;
            return true;
        }
    }

    // MISS → choose victim based on LFU
    auto &lines = sets[set];
    auto victim = std::min_element(lines.begin(), lines.end(),
        [](const CacheLine &a, const CacheLine &b) {
            if (!a.valid) return true;  // Prefer empty slots
            if (!b.valid) return false;
            return a.freq < b.freq;    // ⭐ LFU: least freq first
        });

    // Replace victim
    victim->valid = true;
    victim->tag = tag;
    victim->freq = 1;      // ⭐ new line starts with freq=1
    victim->last_used = time;

    return false;
}

void Cache::dump() const {
    std::cout << "\n=== CACHE DUMP (LFU victim selection active) ===\n";
    for (size_t i = 0; i < num_sets; i++) {
        std::cout << "Set " << i << ": ";
        for (const auto &line : sets[i]) {
            if (line.valid)
                std::cout << "V(tag=" << line.tag << ", f=" << line.freq << ") ";
            else
                std::cout << "I ";
        }
        std::cout << "\n";
    }
}

void Cache::stats() const {
    size_t total_accesses = 0;
    size_t hit_count = 0;

    for (const auto &set : sets) {
        for (const auto &line : set) {
            if (line.valid) {
                total_accesses += line.freq;
                if (line.freq > 1) hit_count += (line.freq - 1);
            }
        }
    }

    double hit_ratio = total_accesses ? (double)hit_count / total_accesses * 100.0 : 0.0;

    std::cout << "\n=== CACHE STATS (LFU) ===\n";
    std::cout << "Total accesses: " << total_accesses << "\n";
    std::cout << "Cache hits: " << hit_count << "\n";
    std::cout << "Hit ratio: " << std::fixed << hit_ratio << "%\n";
}

