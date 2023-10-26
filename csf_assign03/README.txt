Cameron Kolisko and Ronald Garcia

Partner method of programming, both of us worked on all features together.


The experiments we ran involved running every possible combination of parameters on the trace files
gcc.trace and swim.trace:
write allocate, write-through, lru, and each different combination of num_sets, 
num_slots, and slot_size for a given cache size of 2048. We were able to test all these
in a finite amount of time because we wrote a program to save the results to a csv to analyze.

Once we analyzed, we found that:

As expected, the fully associative cache had the least amount of total
clock cycles for both files. The type of cache that was predominantly on top in terms of 
clock cycle counts was a write-allocate, write-back, least recently used cache with a block size of 4 bytes.

We believe that write-allocate caches had fewer clock cycles because storing information in the cache
provided a significant enough advantage for accessing data. It also implies that store misses in these files were later used
in other operations (like loads and stores). 

Write-back caches had fewer clock cycles because write-back caches were able to delay the memory penalty caused
by store hits until eviction, which happens significantly less often. This result also implies that things stored into memory were then
modified by operations.

LRU was the best eviction policy for the caches. This was likely because an LRU policy takes into consideration the fact that some
memory addresses were referenced more often than others, and did not evict those frequently used memory addresses. 

Interestingly, all of the caches with the lowest cycle counts for both files had a block_size of 4 bytes. Also, 
the caches with the lowest miss rates all had cycle counts that were much greater than the top 10 caches 
with the least clock cycle counts (over an order of magnitude worse) AND notably had large block sizes, namely
64 and 128 bytes. This implies that the memory storage penalty for loading blocks from memory outweighed the 
benefit of having more spacially proximate memory addresses in the cache.

We found that the best caches were typically as close to set associative as possible in that they had a lot of
blocks per set (given they were write-allocate write-back lru with byte size of 4). However, non-fully associative
caches were able to perform at a similar level. For example, in the gcc.trace file, a cache with 8 sets and 64 blocks
per set had 5.42 million clock cycles, which is only 4% more clock cycles than the fully associative cache. 
For the swim.trace file, that same cache was only about 15% slower than a fully associative cache. From this,
we can conclude that you do not need a fully associative cache to have an efficient cache implementation.