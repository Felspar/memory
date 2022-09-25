# Allocators


Allocator implementations are split into various layers. Generally there will be a *strategy* header that contains the allocation implementation against whatever tracking data structures it requires.

This can be combined with *storage* options to implement an allocator. What storage options are available varies from allocator to allocator.

The allocators are all PMR ones, that is, they're based on the [polymorphic memory resource](https://en.cppreference.com/w/cpp/memory/memory_resource) found in the `std::pmr` name space.


## `felspar::memory::auto_pool::pmr`

This uses a dynamic pool of fixed size blocks together with a backing allocator that is used for oversized blocks, or when there are no more blocks in the pool.
