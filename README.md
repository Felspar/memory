# Felspar Memory Utilities

**Memory utilities for C++ 20**

[![Documentation](https://badgen.net/static/docs/felspar.com)](https://felspar.com/memory/)
[![GitHub](https://badgen.net/badge/Github/felspar-memory/green?icon=github)](https://github.com/Felspar/memory)
[![License](https://badgen.net/github/license/Felspar/memory)](https://github.com/Felspar/memory/blob/main/LICENSE_1_0.txt)
[![Discord](https://badgen.net/badge/icon/discord?icon=discord&label)](https://discord.gg/tKSabUa52v)

## `accumulation_buffer`

A shared buffer into which data can be added at the end and a `shared_buffer` can be consumed from the front.


## `any_buffer`

An `std::any` like type that can also be used with types that are not copyable. Items can only be placed in it when it's created


## `atomic_pen`

Similar to `holding_pen` and a `std::atomic`. It includes a mutex for controlling access to the value which means it lifts the type requirements that `std::atomic` imposes.


## `hexdump`

A function that takes a `std::span<std::byte>` and prints a hex dump of the memory content to the supplied stream.

```cpp
hexdump(std::cout, span);
```


## `holding_pen`

An `std::optional` like type that cannot be used to change a stored value, only placing a value when it's empty and then emptying again. This allows it to be used with movable types that are not assignable.


## `raw_storage`

A simple type that abstracts the storage requirements for a type where the user tracks whether the storage is in use or not.


## `shared_buffer` and `shared_buffer_view`

A region of contiguous memory that whose ownership is reference counted.


## `small_ring`

A small ring buffer that spills from the back when items are added to the front when full. Storage is embedded within the data structure using a compile time size.


## `small_vector`

A `std::vector` like type that has a single compile time specified capacity.


## `spaceship`

A function that implements three way strong ordering comparison for `std::span`s of values.


## `stable_vector`

A vector whose elements have stable memory addresses even when the vector grows. Internally it is non-contiguous, using a vector of fixed-size sections. Memory is not reclaimed on `clear()`, making repeated use efficient by reusing the previously allocated sections.


## `shared_vector` and `shared_view`

A reference-counted raw memory vector. Multiple `shared_vector` instances may share the same underlying allocation; updates to the reference count are thread-safe but access to the data is not. A `shared_view` is a non-owning slice that can be promoted back to a `shared_vector` without any additional allocation. The alias `shared_bytes` is provided for `shared_vector<std::byte>`.


## `slab_storage`

A bump-pointer allocator whose storage is embedded in the object. Each allocation advances a cursor and is aligned to the configured alignment. Deallocation is a no-op; all memory is freed when the `slab_storage` is destroyed.


## `stack_storage`

A basic allocator whose memory is embedded in the allocator itself. Supports individual deallocations via a best-fit free-list. Not thread-safe. It is not intended to be used as a drop-in allocator in `std::` containers.


## Allocator strategies

A family of composable allocation strategies, all satisfying the `allocator_strategy` concept. Strategies can be combined using the compositor types below.

### Concepts (`concepts.hpp`)

- `allocator_strategy` — requires `allocate(bytes)` and `deallocate(ptr, bytes)`
- `nullable_allocator_strategy` — extends the above with `try_allocate(bytes)` which returns `nullptr` on failure rather than throwing
- `owning_allocator_strategy` — extends nullable with `owns(ptr) noexcept`
- `overallocating_allocator_strategy` — extends the base with `allocate_at_least(bytes)` and `try_allocate_at_least(bytes)` returning an `allocation_result{ptr, actual_bytes}`

`allocation_result` mirrors C++23 `std::allocation_result` and is always available.

### `new_delete_strategy`

A stateless strategy that delegates directly to `::operator new` and `::operator delete`. Satisfies `allocator_strategy`. Useful as a fallback in compositor strategies.

### `pool_strategy` and `pool`

A free-list pool with multiple size-class tiers. Allocations are served from the smallest fitting tier; oversized requests fall through to a configurable fallback. Satisfies `nullable_allocator_strategy` and `overallocating_allocator_strategy`.

```cpp
// Convenience alias with new/delete fallback:
felspar::memory::pool<8, 16, 64> ps;

// Full form with custom fallback:
felspar::memory::pool_strategy<std::array<std::size_t, 3>{8, 16, 64}, MyFallback> ps{fallback};
```

Sizes must be listed in strictly ascending order. `deallocate` may throw if a free-list vector needs to grow; call `reserve(tier, n)` to pre-allocate capacity.

### `fallback_strategy`

A compositor that tries a primary strategy first and falls back to a secondary when the primary returns `nullptr`. The primary must satisfy `owning_allocator_strategy`; deallocation is routed using `owns()`.

### `bitmap_strategy`

A fixed-block pool using a bitmap integer to track which blocks are free. All allocations are rounded up to the block size. Satisfies `owning_allocator_strategy` and `overallocating_allocator_strategy`. The bitmap type parameter controls the number of blocks (e.g. `std::uint64_t` gives 64 blocks).

### `size_split_strategy`

A compositor that routes allocations to a "small" strategy when the request is at or below a threshold and to a "large" strategy otherwise. Deallocation is routed by the same threshold. Safe to use with PMR and stdlib wrappers because they preserve the original allocation size.


## `control` and `owner_tracking_iterator`

Low-level building blocks used by `shared_buffer` and `shared_vector`. `control` is a thread-safe reference-counting block with virtual `free()`; it supports both wrapping an existing object (`wrap_existing`) and allocating a contiguous header+data block (`allocate`). `owner_tracking_iterator` wraps any iterator and carries a pointer to the owning control block, preserving shared ownership across iterator operations.
