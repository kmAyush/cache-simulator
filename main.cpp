#include "CacheSim.h"

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: ./CacheSim <trace_file>\n";
    return 1;
  }

  std::string trace_file(argv[1]);

  unsigned block_size = 16;       // Block size: 16 bytes
  unsigned associativity = 1;    // Direct-mapped
  unsigned cache_capacity = 16384; // Cache size: 16 KB
  unsigned miss_penalty = 30;    // Miss penalty: 30 cycles
  unsigned dirty_wb_penalty = 2; // Dirty write-back penalty: 2 cycles

  CacheSimulator cache_simulator(trace_file, block_size, associativity, cache_capacity, miss_penalty, dirty_wb_penalty);
  cache_simulator.run();

  return 0;
}