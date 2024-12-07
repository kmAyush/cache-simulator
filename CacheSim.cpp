#include "CacheSim.h"
#include <bit>
#include <cassert>
#include <algorithm>

// Constructor
CacheSimulator::CacheSimulator(std::string input_file, unsigned block_size, unsigned associativity,
                               unsigned cache_size, unsigned miss_penalty, unsigned dirty_writeback_penalty)
    : block_size_(block_size), associativity_(associativity), cache_capacity_(cache_size),
      miss_penalty_(miss_penalty), dirty_writeback_penalty_(dirty_writeback_penalty) {
  
  // Input trace file
  trace_file.open(input_file);
  assert(trace_file.is_open() && "Failed to open trace file!");

  // Total number of blocks in the cache
  auto block_count = cache_capacity_ / block_size_;

  // Total number of sets in the cache
  auto set_count = cache_capacity_ / (block_size_ * associativity_);

  // Resize cache state arrays to match total number of blocks
  tag_store_.resize(block_count);
  dirty_bit_store_.resize(block_count);
  valid_bit_store_.resize(block_count);
  priority_store_.resize(block_count);

  // Number of bits required for block offset
  auto block_bits = std::popcount(block_size_ - 1);
  set_offset_ = block_bits;

  // Set mask to extract set index bits
  set_mask_ = set_count - 1;

  // Bit offset for the tag
  tag_offset_ = block_bits + std::popcount(set_mask_);
}

void CacheSimulator::run() {
  std::string line;
  while (std::getline(trace_file, line)) {
    auto [is_write, address, instructions] = parse_trace_line(line);
    auto [hit, dirty_writeback] = access_cache(is_write, address);
    update_statistics(instructions, is_write, hit, dirty_writeback);
  }
}

// Destructor
CacheSimulator::~CacheSimulator() {
  trace_file.close();
}

std::tuple<bool, std::uint64_t, int> CacheSimulator::parse_trace_line(const std::string& line) {
  int is_write;
  std::uint64_t address;
  int instructions;
  sscanf(line.c_str(), "# %d %lx %d", &is_write, &address, &instructions);
  return {is_write, address, instructions};
}