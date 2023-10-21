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

        if ((int)buf.size() != 0) {
            trace.push_back(buf);
        }
    }
}


Cache* initialize_cache(uint32_t num_sets, uint32_t num_slots, uint32_t slot_size) {

    Cache* new_cache = new Cache;
    for (uint32_t i = 0; i < num_sets; i++) {
        Set* newSet = new Set;
        for (uint32_t j = 0; j < num_slots; j++) { //Initalize all of the slots in a set with 0s.
            newSet->slots.push_back({0, 0, 0, 0, 0});
        }
        //After initializing a set, put it in the cache.
        new_cache->sets.push_back(*newSet);

        delete newSet;
    }

    new_cache->index_bits = log2_with_pow_2(num_sets); //Custom int log 2 function.
    new_cache->offset_bits = log2_with_pow_2(slot_size);

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
        throw std::invalid_argument("ERROR: load or store instruction not found");
    }
    //Place the number of the address in the address pointer.
    ss >> buffer;
    *address = std::stol(buffer, 0, 16);

    return is_load;
}

int handle_address(Cache* cache, bool write_allocate, bool is_write_through, bool is_load, bool is_lru, uint32_t address, int* hit_count, int* miss_count, int slot_size) {

    if (write_allocate) { //If write allocate lru, go to that function.
        return handle_write_allocate(is_lru, cache, is_load, is_write_through, address, hit_count, miss_count, slot_size);
    } else { //Else, it's a no write allocate lru, call that function.
        return handle_no_write_allocate(is_lru, cache, is_load, address, hit_count, miss_count, slot_size);
    }

    return 0;
}

/**
 * @param cache is the cache.
 * @param is_load is if its a load or not.
 * @param address number representing adress in memory.
 * @return number of clock cycles
*/
int handle_write_allocate(bool is_lru, Cache* cache, bool is_load, bool is_write_through, uint32_t address, int* load_hit_count, int* store_hit_count, int slot_size) {

    uint32_t tag = calc_tag_bits(address, cache); //Tag is the left over bits after index and offset.
    Set* set = find_set(cache, address);
    Slot* eviction_slot = &set->slots.at(0);
    int penalty = slot_size / 4;

    // loop through each slot of the set
    for (int i = 0; i < (int)set->slots.size(); i++) {
        Slot* cur_slot = &set->slots.at(i);

        // if there is a match, then its a hit
        if (cur_slot->valid && cur_slot->tag == tag) {
            return handle_hit_write_allocate(cur_slot, is_load, is_write_through, load_hit_count, store_hit_count, penalty);
        } 
        
        eviction_slot = choose_eviction(eviction_slot, cur_slot, is_lru);
        
        // if there is an invalid block, we have a miss, but open space. Add to slot.
        // For write-allocate, load and store both load into cache.
        if (!cur_slot->valid) {
            slot_in(cur_slot, tag);
            return 2 + penalty * MEMORY_PENALTY;
        }
    }
    // if here, all slots were valid and none of them had the same tag (miss).
    // Evict the least recently used slot.
    bool write_back_dirty_block = !is_write_through && eviction_slot->is_dirty;
    
    slot_in(eviction_slot, tag);

    // if the slot being evicted is dirty and we are a write back, 
    // additional penalty of storing the dirty block on top of miss.
    if (write_back_dirty_block) {
        return 4 + 2 * penalty * MEMORY_PENALTY;
    }
    // if write through, write to memory and save into cache (2)
    return 2 + penalty * MEMORY_PENALTY;
}

int handle_no_write_allocate(bool is_lru, Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count, int slot_size) {

    uint32_t tag = calc_tag_bits(address, cache); //Tag is the left over bits after index and offset.
    Set* set = find_set(cache, address);
    Slot* eviction_slot = &set->slots.at(0);
    int penalty = slot_size / 4;

    // loop through each slot of the set
    for (int i = 0; i < (int)set->slots.size(); i++) {
        Slot* cur_slot = &set->slots.at(i);

        // if there is a match, then its a hit
        if (cur_slot->valid && cur_slot->tag == tag) { //Hits return immediately.
            return handle_hit_no_write_allocate(cur_slot, is_load, load_hit_count, store_hit_count, penalty);
        } 

        eviction_slot = choose_eviction(eviction_slot, cur_slot, is_lru);
        
        // if there is an available block, add into it. This is a miss.
        if (!cur_slot->valid) {

            // if it is a load, add to cache like normal.
            if (is_load) {
                slot_in(cur_slot, tag);
                return 1 + penalty * MEMORY_PENALTY;
            }

            // if it is a store, write to memory (but do not write to cache)
            return penalty * MEMORY_PENALTY;
        }
    }

    // if here, all slots were valid and none of them had the same tag
    // evict the least recently used slot (miss). If a store, do not evict
    // (because nothing is being saved), so only storing to memory
    if (is_load) {
        slot_in(eviction_slot, tag);
        return 1 + penalty * MEMORY_PENALTY;
    } 
    return penalty * MEMORY_PENALTY;
}

/**
 * @param cache is the cache.
 * @param address is the memory address being accessed.
 * @return address of set found.
*/
Set* find_set(Cache* cache, uint32_t address) {
    return &cache->sets.at(calc_index_bits(address, cache));
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

int handle_hit_write_allocate(Slot* slot, bool is_load, bool is_write_through, int* load_hit_count, int* store_hit_count, int penalty) {
    slot->access_ts = TIME++;

    if (is_load) {
        (*load_hit_count)++;
        return 1;
    }

    (*store_hit_count)++;

    if (is_write_through) {
        return penalty * MEMORY_PENALTY;
    }

    slot->is_dirty = true;
    return 1;
}

int handle_hit_no_write_allocate(Slot* slot, bool is_load, int* load_hit_count, int* store_hit_count, int penalty) {
    slot->access_ts = TIME++;

    if (is_load) {
        (*load_hit_count)++;
        return 1;
    }

    (*store_hit_count)++;

    return penalty * MEMORY_PENALTY;
}

void slot_in(Slot* slot, uint32_t tag) {
    slot->valid = true;
    slot->tag = tag;
    slot->load_ts = TIME;
    slot->access_ts = TIME++;
    slot->is_dirty = false;
}

Slot* choose_eviction(Slot* cur_eviction, Slot* slot, bool is_lru) {
    if (is_lru) {

        if (cur_eviction->access_ts > slot->access_ts) {
            return slot;
        }

        return cur_eviction;
    }

    if (cur_eviction->load_ts > slot->load_ts) {
        return slot;
    }

    return cur_eviction;
}