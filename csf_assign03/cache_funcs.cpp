#include "cache_funcs.h"
#include <string>
#include <sstream>
#include <exception>
#include <cmath>

void read_file(std::istream &in, std::deque<std::string> &trace) {

    std::string buf;
    while (!in.eof()) {
        getline(in, buf);
        trace.push_back(buf);
    }
}


 Cache* initialize_cache(int num_sets,int set_size) {
    Cache* new_cache = new Cache;
    for (int i = 0; i < num_sets; i++) {
        Set* newSet = new Set;
        for (int j = 0; j < set_size; j++) { //Initalize all of the slots in a set with 0s.
            newSet->slots.push_back({0, 0, 0, 0});
        }
        //After initializing a set, put it in the cache.
        new_cache->sets.push_back(*newSet);
    }
    new_cache->index_bits = log2(1); //Custom int log 2 function.
    return new_cache;
}

/**
 * @param line is the line being parsed
 * @param address is a pointer to an int that stores address in line.
 * @return 1 if load, 0 if store.
*/
bool handle_line(std::string line, int* address) {
    bool is_load;

    //Place string in string stream.
    std::stringstream ss;
    ss.str(line);
    std::string buffer;
    ss >> buffer;

    //See if first token is a load or store command.
    if (buffer.at(0) == 'l' && buffer.length() == 1) {
        is_load = true;
    } else if (buffer.at(0) == 's' && buffer.length() == 1) {
        is_load = false;
    } else {
        throw std::invalid_argument("load or store instruction not found");
    }
    //Place the number of the address in the address pointer.
    ss >> buffer;
    *address = std::stoi(buffer, 0, 16);

    return is_load;
}

/**
 * @param cache is the cache.
 * @param is_load is if its a load or not.
 * @param address number representing adress in memory.
 * @param block_size size of slot, amount of memory loaded.
 * @return number of clock cycles
*/
int write_allocate_write_through_lru(Cache* cache, bool is_load, int address, int block_size) {
    bool hit = check_hit(cache, address, block_size);
}

/**
 * @param cache
 * @param address
 * @param block_size
 * @return if it was a hit or not.
*/
bool check_hit(Cache* cache, int address, int block_size) {
    int index_bits;
    return false;
}