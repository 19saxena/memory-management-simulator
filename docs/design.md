# Memory-Management Simulator — Design Document

## 1. Memory Layout & Assumptions
- Simulates a **single contiguous physical memory space**.
- Memory unit = **1 byte**.
- All allocations are **aligned to 16 bytes**.
- Uses an explicit `Block` header storing:
  - `start address`
  - `aligned size`
  - `free/used status`
  - `block_id`
  - `requested size`
  - `internal fragmentation`
- On `free(block_id)`, adjacent free blocks are **coalesced immediately**.
- External fragmentation is computed from the **largest contiguous free segment**.

### Memory Layout Diagram
```
[ USED | USED | FREE | USED | FREE ]
^ contiguous linear memory
```

---

## 2. Allocation Strategies
Implemented inside the memory manager:

### First Fit
```
Traverse block list → first FREE block with size ≥ request → allocate
```

### Best Fit
```
Traverse → choose smallest FREE block with size ≥ request → allocate
```

### Worst Fit
```
Traverse → choose largest FREE block with size ≥ request → allocate
```

### Allocation Flow Diagram
```
malloc(request)
      ↓
Find suitable FREE block (based on strategy)
      ↓
Align size to 16 bytes
      ↓
Split if larger than needed
      ↓
Mark USED, assign block_id, track fragmentation
```

---

## 3. Buddy Allocator
- Memory initialized using `buddy_init(size)` where size is rounded to next power of 2 if needed.
- Allocation:
  1. Round request to next power of 2
  2. Find smallest available free list block ≥ rounded size
  3. Recursively split until exact size reached
  4. Assign `block_id` and mark USED
- Buddy address is computed using:
```
buddy = start_address XOR block_size
```
- Freeing:
  1. Lookup block by `block_id`
  2. Compute buddy
  3. Merge recursively if buddy is FREE
  4. Insert merged block back into free list

### Buddy Split Diagram
```
128
├─ 64
│  ├─ 32  (allocated)
│  └─ 32  (buddy)
└─ 64
```

### Buddy Merge Diagram
```
free(32 at 0x20)
buddy = 0x20 XOR 32 = 0x00
if FREE → merge into 64
repeat...
```

---

## 4. Cache Simulation
Not implemented, so:
> Cache module is **not part of current build** and not included in interface.

---

## 5. Metrics Tracked
| Metric | Description |
|---|---|
| Memory utilization | `(used / total) × 100` |
| Internal fragmentation | `aligned_size − requested_size` per block |
| External fragmentation | `1 − (largest_free / total_free)` |
| Allocation success rate | `(success / total requests) × 100` |
| Allocation failure rate | `100 − success rate` |

---

## 6. Implemented Modules
- `MemoryManagerSimulator` — allocation + deallocation + dump + stats + visualization
- `BuddyAllocator` — power-of-2 based allocation + buddy split/merge

---

## 7. Current Status
| Feature | Status |
|---|---|
| Physical memory simulation | ✔ Implemented |
| 16-byte aligned dynamic allocation | ✔ Implemented |
| First/Best/Worst fit | ✔ Implemented |
| Free + coalescing | ✔ Implemented |
| Buddy allocator | ✔ Implemented |
| Cache simulation | ✘ Not implemented |
| Virtual memory | ✘ Optional, not implemented |

---

## 8. Limitations
- No caching or paging.
- No disk latency simulation.
- Visualization scale adapts dynamically based on `init memory` size.

---

End of design.
```

````markdown
# Memory-Management Simulator — Design Document

## 1. Memory Layout & Assumptions
- Simulates a **single contiguous physical memory space**.
- Memory unit = **1 byte**.
- All allocations are **aligned to 16 bytes**.
- Uses an explicit `Block` header storing:
  - `start address`
  - `aligned size`
  - `free/used status`
  - `block_id`
  - `requested size`
  - `internal fragmentation`
- On `free(block_id)`, adjacent free blocks are **coalesced immediately**.
- External fragmentation is computed from the **largest contiguous free segment**.

### Memory Layout Diagram
```
[ USED | USED | FREE | USED | FREE ]
^ contiguous linear memory
```

---

## 2. Allocation Strategies
Implemented inside the memory manager:

### First Fit
```
Traverse block list → first FREE block with size ≥ request → allocate
```

### Best Fit
```
Traverse → choose smallest FREE block with size ≥ request → allocate
```

### Worst Fit
```
Traverse → choose largest FREE block with size ≥ request → allocate
```

### Allocation Flow Diagram
```
malloc(request)
      ↓
Find suitable FREE block (based on strategy)
      ↓
Align size to 16 bytes
      ↓
Split if larger than needed
      ↓
Mark USED, assign block_id, track fragmentation
```

---

## 3. Buddy Allocator
- Memory initialized using `buddy_init(size)` where size is rounded to next power of 2 if needed.
- Allocation:
  1. Round request to next power of 2
  2. Find smallest available free list block ≥ rounded size
  3. Recursively split until exact size reached
  4. Assign `block_id` and mark USED
- Buddy address is computed using:
```
buddy = start_address XOR block_size
```
- Freeing:
  1. Lookup block by `block_id`
  2. Compute buddy
  3. Merge recursively if buddy is FREE
  4. Insert merged block back into free list

### Buddy Split Diagram
```
128
├─ 64
│  ├─ 32  (allocated)
│  └─ 32  (buddy)
└─ 64
```

### Buddy Merge Diagram
```
free(32 at 0x20)
buddy = 0x20 XOR 32 = 0x00
if FREE → merge into 64
repeat...
```

---


## 4. Multilevel Cache Simulation

### Cache Hierarchy
- **L1 Cache**
  - Size: 256 bytes
  - Block size: 64 bytes
  - Associativity: Direct-Mapped (1-way)
  - Replacement Policy: **FIFO**
- **L2 Cache**
  - Size: 1024 bytes
  - Block size: 64 bytes
  - Associativity: 4-way Set-Associative
  - Replacement Policy: **LRU**
- Access order:
```
Physical Address → L1 Cache → L2 Cache → Memory (on miss)
```

### Cache Line Model
Each cache line stores:
| Field | Purpose |
|---|---|
| `valid bit` | Whether line contains data |
| `tag` | Block identifier for lookup |
| `frequency counter` | Used by LFU (not used here) |
| `last_used timestamp` | For LRU eviction |
| `insertion order` | For FIFO replacement |

### Cache Access & Miss Propagation
1. **Check L1 Cache**
   - If tag matches a valid line → **HIT**, update metadata.
   - Else → **MISS**
2. **On L1 Miss, check L2 Cache**
   - If found → **L2 HIT**, load block into L1 (propagated upward).
   - Else → **L2 MISS**, request served from memory.
3. **Eviction Policy is applied per-set when inserting new lines**
   - L1 evicts based on **FIFO order**.
   - L2 evicts based on **least-recently used timestamp**.

### Cache Hit/Miss Metrics
The simulator tracks:
- `L1 hits, L1 misses`
- `L2 hits, L2 misses`
- `Hit ratio per level = (hits / total accesses) × 100`
- `Miss penalty propagation is symbolic and occurs only as counter increments`

### Cache Flow Diagram
```
access(addr)
      ↓
Compute tag = addr / block_size
      ↓
Locate set = tag % num_sets
      ↓
L1 HIT? — Yes → return
      ↓ No
L2 HIT? — Yes → increment L2 hits, insert into L1
      ↓ No
Serve from Memory → insert into L2 → insert into L1
```

### Cache Behavior Example
```
access(0)
access(64)
access(0)
access(128)
access(0)
```
Results in:
- L1 hits increasing when repeated tag=0 is found.
- L2 accessed only when L1 misses.
- L2 shows only cold misses if no upward propagation occurred earlier.

---



## 5. Metrics Tracked
| Metric | Description |
|---|---|
| Memory utilization | `(used / total) × 100` |
| Internal fragmentation | `aligned_size − requested_size` per block |
| External fragmentation | `1 − (largest_free / total_free)` |
| Allocation success rate | `(success / total requests) × 100` |
| Allocation failure rate | `100 − success rate` |

---

## 6. Implemented Modules
- `MemoryManagerSimulator` — allocation + deallocation + dump + stats + visualization
- `BuddyAllocator` — power-of-2 based allocation + buddy split/merge

---

## 7. Limitations
- No caching or paging.
- No disk latency simulation.
- Visualization scale adapts dynamically based on `init memory` size.

---

End of design.
```
