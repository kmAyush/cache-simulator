#ifndef CACHESIM_H
#define CACHESIM_H

#include <iostream>
#include <fstream>
#include <string>
#include <span>
#include <cstdint>
#include <algorithm>
#include <bit>
#include <ranges>
#include <tuple>
#include <vector>


class CacheSimulator{
public:
	CacheSimulator(
		std::string input_file,
		unsigned block_size,
		unsigned associativity,
		unsigned cache_size,
		unsigned miss_penalty,
		unsigned dirty_writeback_penalty);
	
	void run();
	~CacheSimulator();

private:
	// Trace file for memory access patterns
	std::ifstream trace_file;

	// Cache configuration parameters
	unsigned block_size_;               // Size of each block in bytes
	unsigned associativity_;            // Associativity of the cache
	unsigned cache_capacity_;           // Total cache size in bytes
	unsigned miss_penalty_;             // Penalty for a cache miss (in cycles)
	unsigned dirty_writeback_penalty_;  // Penalty for a dirty writeback (in cycles)

	// Derived cache configuration
	unsigned set_offset_;               // Offset bits for cache sets
	unsigned tag_offset_;               // Offset bits for cache tags
	unsigned set_mask_;                 // Mask for extracting the set index

	// Cache state data structures
	std::vector<std::uint64_t> tag_store_;      // Stores the tags for each block
	std::vector<char> dirty_bit_store_;         // Tracks dirty bits for each block
	std::vector<char> valid_bit_store_;         // Tracks valid bits for each block
	std::vector<int> priority_store_;           // Tracks replacement priority for each block

	// Performance statistics
	std::int64_t write_count_ = 0;              // Number of write operations
	std::int64_t memory_access_count_ = 0;      // Total memory accesses
	std::int64_t miss_count_ = 0;               // Total cache misses
	std::int64_t dirty_writeback_count_ = 0;    // Number of dirty writebacks
	std::int64_t instruction_count_ = 0;        // Total instructions processed

	void dump_statistics();

	// Parses a single line from the trace file to extract access type, address, and instructions.
	std::tuple<bool, std::uint64_t, int> parse_trace_line(const std::string& line);

	// Simulates a single cache access and updates the cache state.
	std::tuple<bool, bool> access_cache(bool is_write, std::uint64_t address);

	// Extracts the set index from a memory address.
	int extract_set_index(std::uint64_t address);

	// Extracts the tag from a memory address.
	std::uint64_t extract_tag(std::uint64_t address);

	// Updates the performance statistics after a memory access.
	void update_statistics(int instructions, bool is_write, bool hit, bool dirty_writeback);

	void print_statistics();
};

#endif