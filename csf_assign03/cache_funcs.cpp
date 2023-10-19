#include "cache_funcs.h"
#include <string>
#include <sstream>
#include <exception>
#include <cmath>

uint64_t TIME = 0;

void read_file(std::istream &in, std::deque<std::string> &trace) {

    std::string buf;
    while (!in.eof()) {
        getline(in, buf);
        trace.push_back(buf);
    }
}


 Cache* initialize_cache(int num_sets, int set_size) {
    Cache* new_cache = new Cache;
    for (int i = 0; i < num_sets; i++) {
        Set* newSet = new Set;
        for (int j = 0; j < set_size; j++) { //Initalize all of the slots in a set with 0s.
            newSet->slots.push_back({0, 0, 0, 0});
        }
        //After initializing a set, put it in the cache.
        new_cache->sets.push_back(*newSet);
    }
    new_cache->index_bits = log2_with_pow_2(num_sets); //Custom int log 2 function.
    new_cache->offset_bits = log2_with_pow_2(set_size);
    return new_cache;
}

/** Finds the integer log base two of a power of two.
 * @param num number to find the log base two of
 *      Pre: number is a power of 2.
 * @return log base two of the number.
*/
int log2_with_pow_2(int num) {
    int counter = 0;
    //While power bit is not right most, keep shifting and count shifts.
    while (num != 1) {
        num = num >> 1;
        counter++;
    }
    return counter;
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
    int tag = address >> cache->index_bits + cache->offset_bits; //Tag is the left over bits after index and offset.
    Slot* slot = find_slot(cache, address, block_size);
    if (slot->valid && slot->tag == tag) { //If hit, update timestamp and return 1 clock cycle.
        TIME++; //Update TIME.
        slot->access_ts = TIME;
        return 1; //One clockcycle passed for hit.
    } else { //Otherwise, we had a miss. Update tag, update to valid, remove lru if needed, return 100 clock cycles.
        slot->tag = tag;
        slot->valid = true;
        removeLRU();
    }
}

/** Remove LRU
 * @param cache the cache
*/
void removeLRU(Cache* cache) {
    
}

/**
 * @param cache is the cache
 * @param address is the memory address
 * @return address of slot found.
*/
Slot* find_slot(Cache* cache, uint32_t address) {
    int offset;
    int tag_bits = 32 - cache->index_bits + cache->offset_bits;

    Set* set = find_set(cache, address);

    offset = address << tag_bits + cache->index_bits; //Remove tag and index.
    offset = address >> tag_bits + cache->index_bits; //Move it back to the right.

    return &set->slots.at(offset);
}

/**
 * @param cache is the cache.
 * @param address is the memory address being accessed.
 * @return address of set found.
*/
Set* find_set(Cache* cache, uint32_t address) {
    int index;
    int tag_bits = 32 - cache->index_bits + cache->offset_bits;

    index = (address << tag_bits); //Remove the tag bits.
    index = (index >> (tag_bits) + cache->offset_bits); //Then, remove the offset bits.

    return &cache->sets.at(index);
}