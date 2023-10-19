
#ifndef CACHE_FUNCS
#define CACHE_FUNCS
#include <iostream>
#include <deque>
#include <string>
#include "cache.h"

void read_file(std::istream &in, std::deque<std::string> &trace);


Cache* initialize_cache(uint32_t num_sets,uint32_t assoc_fac, uint32_t line_size);

bool handle_line(std::string line, uint32_t* address);

int write_allocate_write_through_lru(Cache* cache, bool is_load, uint32_t address);

Slot* find_slot(Cache* cache, uint32_t address);

Set* find_set(Cache* cache, uint32_t address);

uint32_t log2_with_pow_2(uint32_t num);

bool try_lru_slot_in(Cache* cache, uint32_t address);

uint32_t calc_index_bits(uint32_t address, Cache* cache);

uint32_t calc_offset_bits(uint32_t address, Cache* cache);

uint32_t calc_tag_bits(uint32_t address, Cache* cache);


#endif
