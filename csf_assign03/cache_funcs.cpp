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

        //if ((int)buf.size() != 0) {
            trace.push_back(buf);
        //}
    }
}


 Cache* initialize_cache(uint32_t num_sets, uint32_t assoc_fac, uint32_t line_size) {

    Cache* new_cache = new Cache;
    for (uint32_t i = 0; i < num_sets * assoc_fac; i++) {
        Set* newSet = new Set;
        for (uint32_t j = 0; j < line_size; j++) { //Initalize all of the slots in a set with 0s.
            newSet->slots.push_back({0, 0, 0, 0});
        }
        //After initializing a set, put it in the cache.
        new_cache->sets.push_back(*newSet);
    }

    new_cache->index_bits = log2_with_pow_2(num_sets); //Custom int log 2 function.
    new_cache->offset_bits = log2_with_pow_2(line_size);
    new_cache->associativity_factor = assoc_fac;

    return new_cache;
}

/** Finds the integer log base two of a power of two.
 * @param num number to find the log base two of
 *      Pre: number is a power of 2.
 * @return log base two of the number.
*/
uint32_t log2_with_pow_2(uint32_t num) {
    uint32_t counter = 0;
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
bool handle_line(std::string line, uint32_t* address) {
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
    *address = std::stol(buffer, 0, 16);

    return is_load;
}

/**
 * @param cache is the cache.
 * @param is_load is if its a load or not.
 * @param address number representing adress in memory.
 * @param block_size size of slot, amount of memory loaded.
 * @return number of clock cycles
*/
int write_allocate_write_through_lru(Cache* cache, bool is_load, uint32_t address) {
    uint32_t tag = address >> (cache->index_bits + cache->offset_bits); //Tag is the left over bits after index and offset.
    Slot* slot = find_slot(cache, address);
    if (slot->valid && slot->tag == tag) { //If hit, update timestamp and return 1 clock cycle.
        slot->access_ts = TIME;
        TIME++; //Update TIME.
        return 1; //One clockcycle passed for hit.
    } else {//Otherwise, we had a miss. Update tag, update to valid, remove lru if needed, return 100 clock cycles.
        try_lru_slot_in(cache, address);
        return 101;
    }

    return 0;
}

/** Function to handle a miss.
 * @param cache the cache.
 * @param address the address of the slot that missed.
 * @return returns true if there was an eviction, false otherwise.
*/
bool try_lru_slot_in(Cache* cache, uint32_t address) {

    uint32_t index = calc_index_bits(address, cache);
    uint32_t offset = calc_offset_bits(address, cache);
    uint32_t tag = calc_tag_bits(address, cache);

    Set* cur_line = &cache->sets.at(index);
    Slot* cur_slot = &cur_line->slots.at(offset);

    Slot* lru_slot = cur_slot;

    for (uint32_t i = 0; i < cache->associativity_factor; i++) {
        // get current cache line
        cur_line = &cache->sets.at(index + i);

        // get the slot at the offset
        cur_slot = &cur_line->slots.at(offset);

        // choose the least recently used slot based on access time stamp
        if (lru_slot->access_ts < cur_slot->access_ts) {
            lru_slot = cur_slot;
        }

        // if there is an open slot, slot in
        if (!cur_slot->valid) {
            cur_slot->valid = true;
            cur_slot->tag = tag;

            // initialized time stamps
            cur_slot->load_ts = TIME;
            cur_slot->access_ts = TIME++;
            return false;
        } 

    }

    // if there were no open slots, evict least recently used and slot in new one
    lru_slot->tag = tag;
    lru_slot->access_ts = TIME;
    lru_slot->load_ts = TIME++;

    return true;
}



/**
 * @param cache is the cache
 * @param address is the memory address
 * @return address of slot found.
*/
Slot* find_slot(Cache* cache, uint32_t address) {
    
    Set* set = find_set(cache, address);

    return &set->slots.at(calc_offset_bits(address, cache));
}

/**
 * @param cache is the cache.
 * @param address is the memory address being accessed.
 * @return address of set found.
*/
Set* find_set(Cache* cache, uint32_t address) {
    return &cache->sets.at(calc_index_bits(address, cache) * cache->associativity_factor);
}

uint32_t calc_tag_bits(uint32_t address, Cache* cache) {

    // gets the bits that make up the tag 
    address >>= (cache->index_bits + cache->offset_bits);
    return address;
}

uint32_t calc_offset_bits(uint32_t address, Cache* cache) {

    // gets rid of the tag bits and the index bits
    address &= ~(~(0U) << (cache->offset_bits));

    return address;
}

uint32_t calc_index_bits(uint32_t address, Cache* cache) {

    // gets rid of the tag bits 
    address &= ~(~(0U) << (cache->offset_bits + cache->index_bits));

    // get rid of the offset bits
    address >>= cache->offset_bits;

    return address;
}