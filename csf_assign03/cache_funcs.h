
#ifndef CACHE_FUNCS
#define CACHE_FUNCS
#include <iostream>
#include <deque>
#include <string>
#include "cache.h"
#define MEMORY_PENALTY 100

void read_file(std::istream &in, std::deque<std::string> &trace);


Cache* initialize_cache(uint32_t num_sets,uint32_t assoc_fac, uint32_t line_size);

bool handle_line(std::string line, uint32_t* address);

int handle_address(Cache* cache, bool write_allocate, bool is_write_through, bool is_load, bool is_lru, uint32_t address, int* hit_count, int* miss_count, int slot_size);

int write_allocate_lru(Cache* cache, bool is_load, bool is_write_through, uint32_t address, int* load_hit_count, int* store_hit_count, int slot_size);

int no_write_allocate_lru(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count, int slot_size);

int write_allocate_fifo(Cache* cache, bool is_load, bool is_write_through, uint32_t address, int* load_hit_count, int* store_hit_count, int slot_size);

int no_write_allocate_fifo(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count, int slot_size);

Slot* find_slot(Cache* cache, uint32_t address);

Set* find_set(Cache* cache, uint32_t address);

uint32_t log2_with_pow_2(uint32_t num);

uint32_t calc_index_bits(uint32_t address, Cache* cache);

uint32_t calc_offset_bits(uint32_t address, Cache* cache);

uint32_t calc_tag_bits(uint32_t address, Cache* cache);


#endif
