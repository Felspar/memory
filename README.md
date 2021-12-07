# Felspar memory utilities

**Memory utilities for C++ 20**


## `any_buffer`

An `std::any` like type that can also be used with types that are not copyable. Items can only be placed in it when it's created


## `atomic_pen`

Similar to `holding_pen` and a `std::atomic`. It includes a mutex for controlling access to the value which means it lifts the type requirements that `std::atomic` imposes.


## `holding_pen`

An `std::optional` like type that cannot be used to change a stored value, only placing a value when it's empty and then emptying again. This allows it to be used with movable types that are not assignable.


## `raw_storage`

A simple type that abstracts the storage requirements for a type where the user tracks whether the storage is in use or not.


## `small_ring`

A small ring buffer that spills from the back when items are added to the front when full. Storage is embedded within the data structure using a compile time size.


## `small_vector`

A `std::vector` like type that has a single compile time specified capacity.


## `spaceship`

A function that implements three way strong ordering comparison for numeric types (integers, floats and pointers).


## `stack_storage`

A basic allocator whose memory is embedded in the allocator itself. It is not intended to be used as a drop in allocator in `std::` containers etc.
