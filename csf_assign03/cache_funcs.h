
#ifndef CACHE_FUNCS
#define CACHE_FUNCS
#include <iostream>
#include <deque>
#include <string>
#include "cashe.h"

void read_file(std::istream &in, std::deque<std::string> &trace);


Cache* initialize_cache(int num_sets,int set_size);

bool handle_line(std::string, int*);

#endif
