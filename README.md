# Felspar memory utilities

**Memory utilities for C++ 20**


## `holding_pen`

An `std::optional` like type that cannot be used to change a stored value, only placing a value when it's empty and then emptying again. This allows it to be used with movable types that are not assignable.


## `raw_storage`

A simple type that abstracts the storage requirements for a type where the user tracks whether the storage is in use or not.


## `small_vector`

A `std::vector` like type that has a single compile time specified capacity.


## `stack_storage`

A basic allocator whose memory is embedded in the allocator itself. It is not intended to be used as a drop in allocator in `std::` containers etc.
