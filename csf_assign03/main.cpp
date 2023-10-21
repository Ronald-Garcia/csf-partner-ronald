
#include <iostream>
#include <exception>
#include <string>
#include <deque>
#include "main.h"
#include "cache_funcs.h"

int main(int argc, char** argv) {

    if (argc != 7) {
        std::cerr << "ERROR: invalid number of arguments (needs 6)." << std::endl;
        return 1;
    }

    // number of sets in cache
    uint32_t num_sets;

    // number of slots per set 
    uint32_t num_slots;

    // number of bytes in a block
    uint32_t slot_size; //This is offset (___DOESNT MATTER___).


    // TODO: consider if user plugs in float (1.5 => 1)
    try {

        // gets the num_sets and tries to parse it as an int
        num_sets = std::stoi(argv[1]);

        // gets the set_size and tries to parse it as an int
        num_slots = std::stoi(argv[2]);

        // gets the block_size and tries to parse it as an int
        slot_size = std::stoi(argv[3]);

    } catch (std::invalid_argument& e) {

        std::cerr << "ERROR: must provide integer values for number arguments 'number of sets', 'number of blocks', and 'number of bytes'." << std::endl;
        return 1;
    }

    if (!is_pos_power_of_two(num_sets)) {
        std::cerr << "ERROR: 'number of sets' must be a positive power of 2." << std::endl;
        return 1;
    } 

    if (!is_pos_power_of_two(num_slots)) {
        std::cerr << "ERROR: 'number of blocks' must be a positive power of 2." << std::endl;
        return 1;
    }

    if (!is_pos_power_of_two(slot_size) || slot_size < 4) {
        std::cerr << "ERROR: 'number of bytes' must be a positive power of 2 and at least 4." << std::endl;
        return 1;
    }

    std::string write_allocate_arg = argv[4];
    std::string write_through_arg = argv[5];
    std::string lru_arg = argv[6];


    bool write_allocate;

    if (write_allocate_arg == "write-allocate") {
        write_allocate = true;
    } else if (write_allocate_arg == "no-write-allocate") {
        write_allocate = false;
    } else {
        std::cerr << "ERROR: invalid argument '" << write_allocate_arg << "'." << std::endl;
        return 1;
    }

    // is write through, otherwise its write back
    bool is_write_through;

    if (write_through_arg == "write-through") {
        is_write_through = true;
    } else if (write_through_arg == "write-back") {
        is_write_through = false;
    } else {
        std::cerr << "ERROR: invalid argument '" << write_through_arg << "'." << std::endl;
        return 1;
    }
    
    // is least recently used eviction policy, otherwise its first in first out (fifo)
    bool is_lru;

    if (lru_arg == "lru") {
        is_lru = true;
    } else if (lru_arg == "fifo") {
        is_lru = false;
    } else {
        std::cerr << "ERROR: invalid argument '" << lru_arg << "'." << std::endl;
        return 1;
    }

    if (!is_write_through && !write_allocate) {
        std::cerr << "ERROR: both write-back and no-write-allocate were specified." << std::endl;
        return 1;
    }

    //Initialize the cache.
    Cache* cache = initialize_cache(num_sets, num_slots, slot_size);

    std::deque<std::string> test_deque;

    read_file(std::cin, test_deque); //Put each mem access into deque.

    int total_cycles = 0;
    int store_count = 0;
    int load_count = 0;
    int load_hit_count = 0;
    int store_hit_count = 0;

    for (std::deque<std::string>::const_iterator it = test_deque.cbegin(); test_deque.cend() != it; it++) { 
        uint32_t address;
        bool is_load = handle_line(*it, &address);

        if (is_load) {
            load_count++;
        } else {
            store_count++;
        }
        total_cycles += handle_address(cache, write_allocate, is_write_through, is_load, is_lru, address, &load_hit_count, &store_hit_count, slot_size);
    }

    print_results(load_count, store_count, load_hit_count, store_hit_count, total_cycles);

    delete cache;
}


int is_pos_power_of_two(int num) {

    if (num <= 0) {
        return 0;
    }
    while (num % 2 == 0) {
        num = num >> 1;
    }

    return num == 1;
}

void print_cache(Cache* cache, int num_slots){
    for (int i = 0; i < (int) cache->sets.size(); i++) {
        for (int j = 0; j < num_slots; j++) {
            std::cout << "SLOT #" << j << " IN SET #" << i << std::endl;
            std::cout << "TAG:\t\t" << cache->sets.at(i).slots.at(j).tag << std::endl;
            std::cout << "ACCESS TIME: \t" << cache->sets.at(i).slots.at(j).access_ts << std::endl;
            std::cout << "LOAD TIME: \t" << cache->sets.at(i).slots.at(j).load_ts << std::endl;
            std::cout << "VALID: \t\t" << cache->sets.at(i).slots.at(j).valid << std::endl;
        }
    }
}
void print_results(int total_loads, int total_stores, int load_hits, int store_hits, int total_cycles) {

    std::cout << "Total loads: " << total_loads << std::endl;
    std::cout << "Total stores: " << total_stores << std::endl;
    std::cout << "Load hits: " << load_hits << std::endl;
    std::cout << "Load misses: " << total_loads - load_hits << std::endl;
    std::cout << "Store hits: " << store_hits << std::endl;
    std::cout << "Store misses: " << total_stores - store_hits << std::endl;
    std::cout << "Total cycles: " << total_cycles << std::endl;
}



