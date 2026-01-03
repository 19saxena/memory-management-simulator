#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
#include <cstdint>   
#include <cstddef>
#include <list>

struct Block {
    size_t start = 0;
    size_t size = 0;
    bool free = true;
    int id = -1;
    size_t requested = 0;
    size_t internal_frag = 0;

    Block() = default;

    Block(size_t s, size_t sz, bool f, int i, size_t req)
        : start(s), size(sz), free(f), id(i), requested(req) {
        internal_frag = sz > req ? sz - req : 0;
    }
};

enum class AllocatorType {
    FIRST_FIT,
    BEST_FIT,
    WORST_FIT
};

class MemoryManagerSimulator {
public:
    MemoryManagerSimulator();

    void init(size_t total_size);
    void setAllocator(AllocatorType type);
    void visualize() const;

    int malloc_block(size_t size);
    bool free_block(int block_id);

    void dump() const;
    void stats() const;

private:
    std::list<Block> blocks;
    AllocatorType allocator;
    size_t memory_size = 0;
    int alloc_success = 0;
    int alloc_fail = 0;
    size_t internal_frag = 0;
    uint64_t next_id = 1;
    int total_requests = 0;
    uint64_t time = 0;

    std::list<Block>::iterator find_first_fit(size_t size);
    std::list<Block>::iterator find_best_fit(size_t size);
    std::list<Block>::iterator find_worst_fit(size_t size);

};

#endif
