#ifndef CACHE
#define CACHE

#include <stdint.h>
#include <vector>


typedef struct Slot {
    uint32_t tag;
    bool valid;
    uint32_t load_ts;
    uint32_t access_ts;

} Slot;

typedef struct set {
    std::vector<Slot> slots;
} Set;

typedef struct {
    std::vector<Set> sets;
    int offset_bits;
    int index_bits;
    int block_size;
} Cache;

#endif
