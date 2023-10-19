
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

    if (is_lru) {
        std::cout << "User chose lru :D" << std::endl;
    }

    //Initialize the cashe.
    Cache* cache = initialize_cache(num_sets, num_slots, slot_size);

    print_cache(cache, slot_size);

    std::deque<std::string> test_deque;

    read_file(std::cin, test_deque); //Put each mem access into deque.

    int number_of_clock_cycles;

    for (std::deque<std::string>::const_iterator it = test_deque.cbegin(); test_deque.cend() != it; it++) { 

        std::cout << *it << std::endl;

        uint32_t address;
        bool is_load = handle_line(*it, &address);


        number_of_clock_cycles += write_allocate_write_through_lru(cache, is_load, address);

    }
    print_cache(cache, slot_size);

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

void print_cache(Cache* cache, int line_size){
    for (int i = 0; i < (int) cache->sets.size(); i++) {
        for (int j = 0; j < line_size; j++) {
            std::cout << "SLOT AT LINE " << i << " AND OFFSET " << j << std::endl;
            std::cout << "TAG:\t\t" << cache->sets.at(i).slots.at(j).tag << std::endl;
            std::cout << "ACCESS TIME: \t" << cache->sets.at(i).slots.at(j).access_ts << std::endl;
            std::cout << "LOAD TIME: \t" << cache->sets.at(i).slots.at(j).load_ts << std::endl;
            std::cout << "VALID: \t\t" << cache->sets.at(i).slots.at(j).valid << std::endl;
        }
    }
}


