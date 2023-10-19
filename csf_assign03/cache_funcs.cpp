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

int handle_address(Cache* cache, bool write_allocate, bool is_write_through, bool is_load, bool is_lru, uint32_t address, int* hit_count, int* miss_count) {

    // MILESTONE 2
    if (is_lru) {
        if (write_allocate) {
            if (is_write_through) {
                return write_allocate_lru(cache, is_load, is_write_through, address, hit_count, miss_count);
            } else {

            }
        } else {

        }
    }

    return 0;
}


/**
 * @param cache is the cache.
 * @param is_load is if its a load or not.
 * @param address number representing adress in memory.
 * @return number of clock cycles
*/
int write_allocate_lru(Cache* cache, bool is_load, bool is_write_through, uint32_t address, int* load_hit_count, int* store_hit_count) {

    uint32_t tag = calc_tag_bits(address, cache); //Tag is the left over bits after index and offset.
    Set* set = find_set(cache, address);
    Slot* lru_slot = &set->slots.at(0);

    // loop through each slot of the set
    for (int i = 0; i < (int)set->slots.size(); i++) {
        Slot* cur_slot = &set->slots.at(i);

        // if there is a match, then its a hit
        if (cur_slot->valid && cur_slot->tag == tag) {

            // if a hit, a slot has been accessed
            cur_slot->access_ts = TIME++;

            // a load hit loads from cache (no penalty)
            if (is_load) {
                (*load_hit_count)++;
                return 0;
            } else { //Else, it's a store hit.
            (*store_hit_count)++;
                
                // if a write-back, just modify cur value (no penalty)
                if (!is_write_through) {
                    cur_slot->is_dirty = true;
                    return 0;
                } else { 
                    // if a store hit in a write-through, write it to memory immediately (100 clock cycle penalty)
                    return MEMORY_PENALTY;
                }
            }
        } else { //Otherwise, we have not hit yet.

            // track of the least recently used slot
            if (lru_slot->access_ts > cur_slot->access_ts) {
                lru_slot = cur_slot;
            }
            
            // if there is an invalid block, we have a miss, but open space. Add to slot.
            // For write-allocate, load and store both load into cache.
            if (!cur_slot->valid) {
                cur_slot->valid = true;
                cur_slot->tag = tag;
                cur_slot->load_ts = TIME;
                cur_slot->access_ts = TIME++;
                //If a load, mark not dirty and return.
                if (is_load) {
                    cur_slot->is_dirty = false;
                    return MEMORY_PENALTY;
                } else if (is_write_through) { //If write through, double penalty from store to mem.
                    cur_slot->is_dirty = false;
                    return MEMORY_PENALTY * 2;
                } else { //Otherwise, write back, so regular mem penalty, but dirty block.
                    cur_slot->is_dirty = true;
                    return MEMORY_PENALTY;
                }
            }
        }
    }
    // if here, all slots were valid and none of them had the same tag (miss).
    // Evict the least recently used slot.
    bool write_back_dirty_block = !is_write_through && lru_slot->is_dirty;
    
    lru_slot->tag = tag;
    lru_slot->load_ts = TIME;
    lru_slot->access_ts = TIME++;

    //If its a write-back, that black is dirty. Otherwise, clean.
    if (!is_write_through) {
        lru_slot->is_dirty = true;
    } else {
        lru_slot->is_dirty = false;
    }

    // if the slot being evicted is dirty and we are a write back, 
    // additional penalty of storing the dirty block on top of miss.
    if (write_back_dirty_block) {
        return 2 * MEMORY_PENALTY;
    }

    // if write through, just write to memory, because you can just evict it.
    return MEMORY_PENALTY;
}

int no_write_allocate_lru(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count) {

    uint32_t tag = calc_tag_bits(address, cache); //Tag is the left over bits after index and offset.
    Set* set = find_set(cache, address);
    Slot* lru_slot = &set->slots.at(0);

    // loop through each slot of the set
    for (int i = 0; i < (int)set->slots.size(); i++) {
        Slot* cur_slot = &set->slots.at(i);

        // if there is a match, then its a hit
        if (cur_slot->valid && cur_slot->tag == tag) { //Hits return immediately.

            // if a hit, a slot has been accessed
            cur_slot->access_ts = TIME++;

            // a load hit is loads from cache (no penalty)
            if (is_load) {
                (*load_hit_count)++;
                return 0;
            } else {
                (*store_hit_count)++;
                // if a no-write-allocate, has to be a write through cache
                return MEMORY_PENALTY;
            }
        }

        // track of the least recently used slot
        if (lru_slot->access_ts < cur_slot->access_ts) {
            lru_slot = cur_slot;
        }
        
        // if there is an invalid block, add the slot (a miss)
        if (!cur_slot->valid) {

            // if it is a store, do not add to cache.
            if (is_load) {
                cur_slot->valid = true;
                cur_slot->tag = tag;
                cur_slot->load_ts = TIME;
                cur_slot->access_ts = TIME++;
                cur_slot->is_dirty = false;
            }
            return MEMORY_PENALTY - !is_load;
        }
    }

    // if here, all slots were valid and none of them had the same tag
    // evict the least recently used slot (miss). If a store, do not evict
    // (because nothing is being saved)
    if (is_load) {
        lru_slot->tag = tag;
        lru_slot->load_ts = TIME;
        lru_slot->access_ts = TIME++;
        lru_slot->is_dirty = false;
    }   

    return MEMORY_PENALTY;
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