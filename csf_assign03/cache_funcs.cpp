#include "cache_funcs.h"
#include <string>
#include <sstream>
#include <exception>
#include <cmath>

/**********************
 * TIME STAMP TRACKER *
 **********************
 */
uint64_t TIME = 0;

/*
 **********************
 * READING FILE FUNCS *
 **********************
 */

void read_file(std::istream &in, std::deque<std::string> &trace) {

    std::string buf;
    while (!in.eof()) {
        getline(in, buf);

        if ((int)buf.size() != 0) {
            trace.push_back(buf);
        }
    }
}

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

/*
 ***************
 * CACHE FUNCS *
 ***************
 */

Cache* initialize_cache(uint32_t num_sets, uint32_t num_slots, uint32_t slot_size, bool write_allocate, bool is_write_through, bool is_lru) {

    Cache* new_cache = new Cache;
    for (uint32_t i = 0; i < num_sets; i++) {
        Set* newSet = new Set;
        for (uint32_t j = 0; j < num_slots; j++) { //Initalize all of the slots in a set with 0s.
            Slot* slot = new Slot;
            slot->access_ts = 0;
            slot->tag = 0;
            slot->load_ts = 0;
            slot->is_dirty = 0;
            slot->valid = 0;
            newSet->slots.push_back(slot);
        }
        //After initializing a set, put it in the cache.
        new_cache->sets.push_back(newSet);
    }

    new_cache->index_bits = log2_with_pow_2(num_sets); //Custom int log 2 function.
    new_cache->offset_bits = log2_with_pow_2(slot_size);

    new_cache->options = new Options;

    new_cache->options->write_allocate = write_allocate;
    new_cache->options->is_write_through = is_write_through;
    new_cache->options->is_lru = is_lru;
    new_cache->options->slot_size = slot_size;

    return new_cache;
}

void delete_cache(Cache* cache) {

    int num_sets = cache->sets.max_size();
    int num_slots = cache->sets.at(0)->slots.max_size();

    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < num_slots; j++) {
            delete cache->sets.at(i)->slots.at(j);
        }
        delete cache->sets.at(i);
    }

    delete cache->options;
    delete cache;
}

/*
 *************
 * SET FUNCS *
 *************
 */

Set* find_set(Cache* cache, uint32_t address) {
    return cache->sets.at(calc_index_bits(address, cache));
}

/*
 **************
 * SLOT FUNCS *
 **************
 */

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

/*
 *****************
 * ADDRESS FUNCS *
 *****************
 */

uint32_t calc_index_bits(uint32_t address, Cache* cache) {

    // gets rid of the tag bits 
    address &= ~(~(0U) << (cache->offset_bits + cache->index_bits));

    // get rid of the offset bits
    address >>= cache->offset_bits;

    return address;
}

uint32_t calc_tag_bits(uint32_t address, Cache* cache) {

    // gets the bits that make up the tag 
    address >>= (cache->index_bits + cache->offset_bits);
    return address;
}

int handle_address(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count) {
    Options* options = cache->options;

    if (options->write_allocate) { //If write allocate lru, go to that function.
        return handle_write_allocate(cache, is_load, address, load_hit_count, store_hit_count);
    } else { //Else, it's a no write allocate lru, call that function.
        return handle_no_write_allocate(cache, is_load, address, load_hit_count, store_hit_count);
    }
}

/*
 ******************
 * HELPER METHODS *
 ******************
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

int handle_write_allocate(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count) {

    Options* op = cache->options;

    uint32_t slot_size = op->slot_size;
    bool is_lru = op->is_lru;
    bool is_write_through = op->is_write_through;


    uint32_t tag = calc_tag_bits(address, cache); //Tag is the left over bits after index and offset.
    Set* set = find_set(cache, address);
    Slot* eviction_slot = set->slots.at(0);
    int penalty = slot_size >> 2;

    // loop through each slot of the set
    for (int i = 0; i < (int)set->slots.size(); i++) {
        Slot* cur_slot = set->slots.at(i);

        // if there is a match, then its a hit
        if (cur_slot->valid && cur_slot->tag == tag) {
            return handle_hit_write_allocate(cur_slot, is_load, is_write_through, load_hit_count, store_hit_count);
        } 
        
        // if there is an invalid block, we have a miss, but open space. Add to slot.
        // For write-allocate, load and store both load into cache.
        if (!cur_slot->valid) {
            slot_in(cur_slot, tag);

            if (is_write_through && !is_load) {
                return 1 + penalty * MEMORY_PENALTY + MEMORY_PENALTY;
            }
            return 1 + penalty * MEMORY_PENALTY;
        }

        eviction_slot = choose_eviction(eviction_slot, cur_slot, is_lru);
    }
    // if here, all slots were valid and none of them had the same tag (miss).
    // Evict the least recently used slot.
    bool write_back_dirty_block = !is_write_through && eviction_slot->is_dirty;
    
    slot_in(eviction_slot, tag);

    // if the slot being evicted is dirty and we are a write back, 
    // additional penalty of storing the dirty block on top of miss.
    if (write_back_dirty_block) {
        return 4 + penalty * MEMORY_PENALTY + MEMORY_PENALTY;
    }

    // if write through, write to memory and save into cache (2)
    if (is_write_through && !is_load) {
        return 1 + MEMORY_PENALTY + penalty * MEMORY_PENALTY;
    }

    return 1 + penalty * MEMORY_PENALTY;
}

int handle_no_write_allocate(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count) {

    Options* op = cache->options;

    uint32_t slot_size = op->slot_size;
    bool is_lru = op->is_lru;

    uint32_t tag = calc_tag_bits(address, cache); //Tag is the left over bits after index and offset.
    Set* set = find_set(cache, address);
    Slot* eviction_slot = set->slots.at(0);
    int penalty = slot_size >> 2;

    // loop through each slot of the set
    for (int i = 0; i < (int)set->slots.size(); i++) {
        Slot* cur_slot = set->slots.at(i);

        // if there is a match, then its a hit
        if (cur_slot->valid && cur_slot->tag == tag) { //Hits return immediately.
            return handle_hit_no_write_allocate(cur_slot, is_load, load_hit_count, store_hit_count);
        } 

        
        // if there is an available block, add into it. This is a miss.
        if (!cur_slot->valid) {

            // if it is a load, add to cache like normal.
            if (is_load) {
                slot_in(cur_slot, tag);
                return 1 + penalty * MEMORY_PENALTY;
            }

            // if it is a store, write to memory (but do not write to cache)
            return MEMORY_PENALTY;
        }
        
        eviction_slot = choose_eviction(eviction_slot, cur_slot, is_lru);
    }

    // if here, all slots were valid and none of them had the same tag
    // evict the least recently used slot (miss). If a store, do not evict
    // (because nothing is being saved), so only storing to memory
    if (is_load) {
        slot_in(eviction_slot, tag);
        return 1 + penalty * MEMORY_PENALTY;
    } 
    return MEMORY_PENALTY;
}

int handle_hit_write_allocate(Slot* slot, bool is_load, bool is_write_through, int* load_hit_count, int* store_hit_count) {
    slot->access_ts = TIME++;

    if (is_load) {
        (*load_hit_count)++;
        return 1;
    }

    (*store_hit_count)++;

    if (is_write_through) {
        return 1 + MEMORY_PENALTY;
    }

    slot->is_dirty = true;
    return 1;
}

int handle_hit_no_write_allocate(Slot* slot, bool is_load, int* load_hit_count, int* store_hit_count) {
    slot->access_ts = TIME++;

    if (is_load) {
        (*load_hit_count)++;
        return 1;
    }

    (*store_hit_count)++;

    return MEMORY_PENALTY;
}