#ifndef CACHE
#define CACHE

#include <stdint.h>
#include <vector>


typedef struct Slot {
    uint32_t tag;
    bool valid;
    uint64_t load_ts;
    uint64_t access_ts;
    bool is_dirty;

} Slot;

typedef struct set {
    std::vector<Slot*> slots;
} Set;

typedef struct {
    bool write_allocate;
    bool is_write_through;
    bool is_lru;
    uint32_t slot_size;
} Options;

typedef struct {
    std::vector<Set*> sets;
    uint32_t offset_bits;
    uint32_t index_bits;
    Options* options;
} Cache;


#endif
