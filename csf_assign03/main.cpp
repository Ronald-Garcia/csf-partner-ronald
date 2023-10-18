
#include <iostream>
#include <exception>
#include <string>
#include <deque>
#include "main.h"
#include "cache_funcs.h"
#include "cashe.h"

int main(int argc, char** argv) {

    if (argc != 7) {
        std::cerr << "ERROR: invalid number of arguments (needs 6)." << std::endl;
        return 1;
    }

    // number of sets in cache
    int num_sets;

    // number of blocks in a set
    int set_size;

    // number of bytes in a block
    int block_size;


    // TODO: consider if user plugs in float (1.5 => 1)
    try {

        // gets the num_sets and tries to parse it as an int
        num_sets = std::stoi(argv[1]);

        // gets the set_size and tries to parse it as an int
        set_size = std::stoi(argv[2]);

        // gets the block_size and tries to parse it as an int
        block_size = std::stoi(argv[3]);

    } catch (std::invalid_argument& e) {

        std::cerr << "ERROR: must provide integer values for number arguments 'number of sets', 'number of blocks', and 'number of bytes'." << std::endl;
        return 1;
    }

    if (!is_pos_power_of_two(num_sets)) {
        std::cerr << "ERROR: 'number of sets' must be a positive power of 2." << std::endl;
        return 1;
    } 

    if (!is_pos_power_of_two(set_size)) {
        std::cerr << "ERROR: 'number of blocks' must be a positive power of 2." << std::endl;
        return 1;
    }

    if (!is_pos_power_of_two(block_size) || block_size < 4) {
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
    Cache* cache = initialize_cache(num_sets, set_size);

    std::deque<std::string> test_deque;

    read_file(std::cin, test_deque); //Put each mem access into deque.

    int address;
    bool is_load = handle_line(test_deque.at(0), &address);

    int number_of_clock_cycles;

    number_of_clock_cycles = write_allocate_write_through_lru(cache, is_load, address, block_size);

    std::cout << is_load << " " << address << std::endl;

    for (std::deque<std::string>::const_iterator it = test_deque.cbegin(); test_deque.cend() != it; it++) { 
        std::cout << *it << std::endl;
    }
    
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

void print_cache(Cache* cache, int num_sets, int set_size){
    for (int i = 0; i < num_sets; i++) {
        for (int j = 0; j < set_size; j++) {
            std::cout << cache->sets.at(i).slots.at(j).tag << i << j << std::endl;
        }
    }
}


