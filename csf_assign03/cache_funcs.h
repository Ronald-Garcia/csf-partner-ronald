
#ifndef CACHE_FUNCS
#define CACHE_FUNCS
#include <iostream>
#include <deque>
#include <string>
#include "cache.h"

#define MEMORY_PENALTY 100

/*
 **********************
 * READING FILE FUNCS *
 **********************
 */

/** Function to handle reading a file from an input stream.
 * @param in the input stream.
 * @param trace the deque to save each line.
 */
void read_file(std::istream &in, std::deque<std::string> &trace);

/** Function to handle reading a line of trace.
 * @param line the line of memory trace to be read.
 * @param address the address that is being referenced in the line.
 * @return true if the command was a load, false if the command was a store, 
 *         and save the address referenced in the line into address.
 */
bool handle_line(std::string line, uint32_t* address);

/*
 ***************
 * CACHE FUNCS *
 ***************
 */

/** Function to initialize a cache.
 * @param num_sets the number of sets in the cache.
 * @param num_slots the number of slots in each set.
 * @param slot_size the number of bytes in each slot.
 * @param write_allocate whether or not the cache is a write allocate cache
 * @param is_write_through whether or not the cache is a write through cache
 * @param is_lru whether or not the cache is a lru
 * @return a heap-allocated pointer to a cache.
 */
Cache* initialize_cache(uint32_t num_sets,uint32_t num_slots, uint32_t slot_size, bool write_allocate, bool is_write_through, bool is_lru);


/** Function to deallocate a cache
 * @param cache the cache to delete
 */
void delete_cache(Cache* cache);

/*
 *************
 * SET FUNCS *
 *************
 */

/** Function to find a set associated with the index of the address
 * @param cache the cache to search in
 * @param address the address 
 * @return the pointer to the set
 */
Set* find_set(Cache* cache, uint32_t address);

/*
 **************
 * SLOT FUNCS *
 **************
 */

/** Function to insert a tag into a slot
 * @param slot the slot to insert into
 * @param tag the tag to insert
 */
void slot_in(Slot* slot, uint32_t tag);

/** Function to choose the slot that should be evicted
 * @param cur_eviction the current to-be-evicted slot
 * @param slot the contender
 * @param is_lru the ruleset for choosing an eviction
 * @return the pointer to the slot that should be evicted between the two 
 */
Slot* choose_eviction(Slot* cur_eviction, Slot* slot, bool is_lru);


/*
 *****************
 * ADDRESS FUNCS *
 *****************
 */

/** Function to calculate the index bits of an address
 * @param address the address
 * @param cache the cache ruleset
 * @return the index bits
 */
uint32_t calc_index_bits(uint32_t address, Cache* cache);

/** Function to calculate the tag bits of an address
 * @param address the address
 * @param cache the cache ruleset
 * @return the tag bits
 */
uint32_t calc_tag_bits(uint32_t address, Cache* cache);

/** Function to handle an address with its corresponding command
 * @param cache the cache to store/load to/from
 * @param is_load whether or not the command is a load
 * @param address the address 
 * @param load_hit_count counter to keep track of load hits
 * @param store_hit_count counter to keep track of store hits
 * @return the number of clock cycles it took to complete the action
 */
int handle_address(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count);

/*
 ******************
 * HELPER METHODS *
 ******************
 */

/** Function to return the log_2(num)
 * @param num the number to log
 * @return the result log_2(num)
 */
uint32_t log2_with_pow_2(uint32_t num);

/** Function to handle an operation on a write_allocate cache
 * @param cache the cache 
 * @param is_load whether or not the operation is a load
 * @param address the address being operated on
 * @param load_hit_count a counter to keep track of load hits
 * @param store_hit_count a counter to keep track of store hits
 * @return the clock cycles that it took to perform the operation
 */
int handle_write_allocate(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count);

/** Function to handle an operation on a no_write_allocate cache 
 * @param cache the cache
 * @param is_load whether or not the operation is a load
 * @param address the address being operated on
 * @param load_hit_count a counter to keep track of load hits
 * @param store_hit_count a counter to keep track of store hits
 * @return the clock cycles that it took to perform the operation
 */
int handle_no_write_allocate(Cache* cache, bool is_load, uint32_t address, int* load_hit_count, int* store_hit_count);


/** Function to handle a hit on a write-allocate cache
 * @param slot the slot that was hit
 * @param is_load whether or not the hit was a load or a store
 * @param is_write_through whether or not the cache is write through
 * @param load_hit_count counter to keep track of load hits
 * @param store_hit_count counter to keep track of store hits
 * @return the number of clock cycles that it took to perform the operation
 */
int handle_hit_write_allocate(Slot* slot, bool is_load, bool is_write_through, int* load_hit_count, int* store_hit_count);

/** Function to handle a hit on a no-write-allocate cache
 * @param slot the slot that was hit
 * @param is_load whether or not the hit was a load or a store
 * @param load_hit_count counter to keep track of load hits
 * @param store_hit_count counter to keep track of store hits
 * @return the number of clock cycles that it took to perform the operation
 */
int handle_hit_no_write_allocate(Slot* slot, bool is_load, int* load_hit_count, int* store_hit_count);

#endif
