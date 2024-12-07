# **CPU Cache Simulator**
This project implements a configurable **CPU Cache Simulator** that emulates the behavior of a cache in a computing system. The simulator processes memory access traces and provides insights into cache performance, including:
- **Cache Hits/Misses**
- **Dirty Writebacks**
- **Miss Rate**
- **Instructions Per Cycle (IPC)**

The simulator allows customization of key cache parameters like cache size, associativity, block size, and penalties for cache misses and dirty writebacks.


## **What It Does**
1. Reads a trace file with memory access patterns.
2. Simulates cache operations based on:
   - Cache size
   - Block size
   - Associativity (e.g., direct-mapped, N-way set associative)
3. Tracks performance metrics, including:
   - Total memory accesses
   - Number of cache hits/misses
   - Dirty writebacks
   - Miss rate
   - Total cycles and IPC
4. Outputs instruction-by-instruction details and a final performance summary.

## **How It Works**
The workflow involves the following steps:

1. **Initialization**:
   - Cache state (tags, valid bits, dirty bits) and parameters are initialized based on user-defined configurations.

2. **Processing Input**:
   - The simulator reads the trace file, extracting details for each memory access (e.g., read/write, address, associated instructions).

3. **Cache Access**:
   - For each memory access, the simulator determines the **set index** and **tag** from the memory address.
   - Checks for a **cache hit** or selects a block for eviction on a **cache miss**.
   - Handles dirty writebacks if a modified block is evicted.

4. **Statistics Update**:
   - After each memory access, performance statistics (e.g., total accesses, misses, dirty writebacks) are updated.

5. **Output**:
   - Output details for each memory access and a final summary of cache performance metrics.


```plaintext
Trace File Input → Parse Line → Calculate Set Index & Tag
                   ↓                     ↓
              Access Cache         Cache Miss Handling
                   ↓                     ↓
          Update Cache State   Update Statistics (Miss Rate, IPC)
                   ↓
        Output Results (Instruction Details & Final Summary)
```

## **Run code**

Use `g++` to compile the source files:

```bash
g++ -std=c++20 -o CacheSim CacheSim.cpp main.cpp
```
```bash
./CacheSim trace.txt
```
## **Output**

```plaintext
Instruction 5: READ Address: 0x1000 | Hit: No | Dirty Writeback: No | Instructions: 5
Instruction 8: WRITE Address: 0x2000 | Hit: No | Dirty Writeback: No | Instructions: 3
Instruction 12: READ Address: 0x3000 | Hit: No | Dirty Writeback: No | Instructions: 4
Instruction 18: WRITE Address: 0x1000 | Hit: Yes | Dirty Writeback: No | Instructions: 6
Instruction 20: READ Address: 0x4000 | Hit: No | Dirty Writeback: No | Instructions: 2
Instruction 21: WRITE Address: 0x5000 | Hit: No | Dirty Writeback: Yes | Instructions: 1
Instruction 28: READ Address: 0x1000 | Hit: No | Dirty Writeback: Yes | Instructions: 7
Instruction 32: WRITE Address: 0x2000 | Hit: Yes | Dirty Writeback: No | Instructions: 4
CACHE CONFIGURATION
Cache Size (Bytes): 16384
Associativity: 1
Block Size (Bytes): 16
Miss Penalty (Cycles): 30
Dirty Writeback Penalty (Cycles): 2
MISS RATE: 75%
IPC: 0.150943
```

## **Acknowledgments**
- Special thanks to [Nick from CoffeeBeforeArch](https://github.com/coffeebeforearch) for inspiring this project and sharing knowledge on cache simulators.

