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
    uint32_t offset_bits;
    uint32_t index_bits;
} Cache;

#endif
