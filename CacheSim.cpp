#include "CacheSim.h"
#include <bit>
#include <cassert>
#include <algorithm>
#include <ranges>

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

    std::cout << "Instruction " << instruction_count_ << ": "
              << (is_write ? "WRITE" : "READ") << " "
              << "Address: 0x" << std::hex << address << std::dec
              << " | Hit: " << (hit ? "Yes" : "No")
              << " | Dirty Writeback: " << (dirty_writeback ? "Yes" : "No")
              << " | Instructions: " << instructions << '\n';
  }

  print_statistics();
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

std::tuple<bool, bool> CacheSimulator::access_cache(bool is_write, std::uint64_t address) {
  // Extract set index and tag from memory address
  auto set_index = extract_set_index(address);
  auto tag = extract_tag(address);

  // Base index for this set in the cache state arrays
  auto base_index = set_index * associativity_;

  // Create spans for set-specific data
  std::span set_tags{tag_store_.data() + base_index, associativity_};
  std::span set_dirty_bits{dirty_bit_store_.data() + base_index, associativity_};
  std::span set_valid_bits{valid_bit_store_.data() + base_index, associativity_};
  std::span set_priority{priority_store_.data() + base_index, associativity_};

  // Initialize flags and variables for hit/miss handling
  bool hit = false;
  int replace_index = -1;  // Index of block to replace if miss occurs
  bool dirty_writeback = false; // Indicates if a dirty block is evicted

  // Check all blocks in the set for a hit or invalid entry
  for (unsigned i = 0; i < associativity_; i++) {

    // If block is valid and tags match, it's a cache hit
    if (set_valid_bits[i] && set_tags[i] == tag) {
      hit = true;
      replace_index = i;

      // Mark the block as dirty if it's a write operation
      set_dirty_bits[i] |= is_write;
      break;
    }
    // Track the first invalid block for potential replacement
    if (!set_valid_bits[i] && replace_index == -1) {
      replace_index = i;
    }
  }

  // Handle cache miss if no matching tag was found
  if (!hit) {

    // If no invalid block is available, evict the least prioritized block
    if (replace_index == -1) {
      auto max_priority = std::ranges::max_element(set_priority);
      replace_index = std::distance(set_priority.begin(), max_priority);

      // Record if the evicted block is dirty
      dirty_writeback = set_dirty_bits[replace_index];
    }

    // Replace the block with the new tag and update states
    set_tags[replace_index] = tag;
    set_dirty_bits[replace_index] = is_write;
    set_valid_bits[replace_index] = 1;  // Mark block as valid
  }

  // Update replacement priorities for all blocks in the set
  for (auto& p : set_priority) {
    if (p < associativity_ - 1) p++;  // Increment priority for all blocks
  }
  set_priority[replace_index] = 0;  // Set the accessed/replaced block to the highest priority

  // Return (hit and if dirty block was evicted)
  return {hit, dirty_writeback};
}

// Extract set index by shifting the address and applying the set mask
int CacheSimulator::extract_set_index(std::uint64_t address) {
  return (address >> set_offset_) & set_mask_;
}

// Extract tag by shifting the address to remove set and offset bits
std::uint64_t CacheSimulator::extract_tag(std::uint64_t address) {
  return address >> tag_offset_;
}


void CacheSimulator::update_statistics(int instructions, bool is_write, bool hit, bool dirty_writeback) {
  memory_access_count_++;
  write_count_ += is_write;
  miss_count_ += !hit;
  instruction_count_ += instructions;
  dirty_writeback_count_ += dirty_writeback;
}

void CacheSimulator::print_statistics() {
  std::cout << "CACHE CONFIGURATION\n";
  std::cout << "Cache Size (Bytes): " << cache_capacity_ << '\n';
  std::cout << "Associativity: " << associativity_ << '\n';
  std::cout << "Block Size (Bytes): " << block_size_ << '\n';
  std::cout << "Miss Penalty (Cycles): " << miss_penalty_ << '\n';
  std::cout << "Dirty Writeback Penalty (Cycles): " << dirty_writeback_penalty_ << '\n';

  double miss_rate = (double)miss_count_ / memory_access_count_ * 100.0;
  auto total_cycles = miss_penalty_ * miss_count_ + instruction_count_;
  double ipc = (double)instruction_count_ / total_cycles;

  std::cout << "MISS RATE: " << miss_rate << "%\n";
  std::cout << "IPC: " << ipc << '\n';
}
