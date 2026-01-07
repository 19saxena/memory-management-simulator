# Memory Management Simulator 

A C++ CLI simulator implementing:
- **Memory Allocation:** First Fit, Best Fit, Worst Fit (16-byte alignment, block splitting, and free-block coalescing)
- **Buddy Allocator:** Power-of-2 rounding, recursive splitting, and XOR-based buddy coalescing
- **Multilevel CPU Cache:** L1 (**FIFO**), L2 (**LRU**) with hit/miss counters and miss propagation

---

## Project Structure

src/allocator/ → Allocation algorithms + memory stats
src/buddy/ → Buddy allocator 
src/cache/ → Multilevel cache simulation
tests/ → Workload command scripts (.txt)
logs/ → Pre-generated execution logs
Makefile → build & run automation

---

## Build
mingw32-make
## Run workload tests
memsim.exe < tests/workload_seq.txt

For detailed documentation, please go to docs/design.md
