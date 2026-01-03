#include <iostream>
#include <string>
#include "allocator/memory_manager.h"
#include "buddy/buddy_allocator.h"
#include "cache/cache.h"
#include "cache/multilevel_cache.h"

int main() {
    MemoryManagerSimulator mm;
    std::string cmd;

    // ⭐ Create L1 and L2 cache objects
    Cache L1(256, 64, 1, CachePolicy::FIFO);
    Cache L2(1024, 64, 4, CachePolicy::LRU);
  // 4-way set associative LRU
    MultiLevelCache cache(L1, L2);                // multilevel wrapper

    std::cout << "Memory Management Simulator Ready. Type 'help' for commands.\n";
    BuddyAllocator* ba = nullptr;
    while (true) {
        std::cout << "> ";
        std::cin >> cmd;

        if (cmd == "help") {
            std::cout << "\nCommands:\n";
            std::cout << "  init memory <size>\n";
            std::cout << "  set allocator <first_fit | best_fit | worst_fit>\n";
            std::cout << "  malloc <size>\n";
            std::cout << "  free <block_id>\n";
            std::cout << "  dump memory\n";
            std::cout << "  visualize\n";
            std::cout << "  stats\n";
            std::cout << "  access <address>   (simulate cache access)\n";
            std::cout << "  cache dump\n";
            std::cout << "  cache stats\n";
            std::cout << "  exit\n\n";
        }

        else if (cmd == "init") {
            std::string type;
            std::cin >> type;
            if (type == "memory") {
                size_t size;
                std::cin >> size;
                mm.init(size);
                std::cout << "Initialized physical memory with " << size << " bytes\n";
            }
        }

        else if (cmd == "set") {
            std::string type, policy;
            std::cin >> type >> policy;
            if (type == "allocator") {
                if (policy == "first_fit") mm.setAllocator(AllocatorType::FIRST_FIT);
                else if (policy == "best_fit") mm.setAllocator(AllocatorType::BEST_FIT);
                else if (policy == "worst_fit") mm.setAllocator(AllocatorType::WORST_FIT);
                else std::cout << "Unknown allocator policy\n";
            }
        }

       else if (cmd == "malloc") {
        size_t size;
        std::cin >> size;
        int id = mm.malloc_block(size);

        if (id == -1)
            std::cout << "Allocation failed\n";
        else
            std::cout << "Allocated block id=" << id << " at address assigned internally\n";
    }


        else if (cmd == "free") {
            int id;
            std::cin >> id;
            if (mm.free_block(id))
                std::cout << "Block " << id << " freed and coalesced if possible\n";
            else
                std::cout << "Invalid block id\n";
        }

        else if (cmd == "dump") {
            std::string sub;
            std::cin >> sub;
            if (sub == "memory") mm.dump();
        }

        else if (cmd == "visualize") {
            mm.visualize();
        }

        else if (cmd == "stats") {
            if (ba) ba->stats();
            else mm.stats();
        }

        else if (cmd == "access") {
            size_t address;
            std::cin >> address;
            cache.access(address);  // cache simulator call
        }

        else if (cmd == "cache") {
            std::string sub;
            std::cin >> sub;
            if (sub == "dump") cache.dump();
            else if (sub == "stats") cache.stats();
        }
       else if (cmd == "buddy_init") {
        size_t size;
        std::cin >> size;
        if (ba) delete ba;
        ba = new BuddyAllocator(size);
        std::cout << "Buddy allocator initialized with " << size << " bytes\n";
    }
    else if (cmd == "buddy_malloc") {
        size_t size;
        std::cin >> size;
        if (!ba) std::cout << "Buddy allocator not initialized!\n";
        else ba->allocate(size);
    }
    else if (cmd == "buddy_free") {
        int id;
        std::cin >> id;
        if (!ba) std::cout << "Buddy allocator not initialized!\n";
        else ba->freeBlock(id);
    }
    else if (cmd == "buddy_dump") {
        if (!ba) std::cout << "Buddy allocator not initialized!\n";
        else ba->dump();
    }

        else if (cmd == "exit") {
            break;
        }
    }
}
