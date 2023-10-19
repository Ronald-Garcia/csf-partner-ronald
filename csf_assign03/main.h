
// helper methods for main file

#ifndef MAIN_H
#define MAIN_H

#include "cache.h"

int is_pos_power_of_two(int num);

void print_cache(Cache* cache, int line_size);

void print_results(int total_loads, int total_stores, int load_hits, int store_hits, int total_cycles);

#endif // MAIN_H