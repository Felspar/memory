#pragma once


#include <cstddef>


namespace felspar::memory {


    /// ## Result type for over-allocating strategies
    /**
     * Mirrors `std::allocation_result` from C++23 but is always available.
     * `ptr` is the allocated block; `bytes` is the actual allocation size
     * (>= the amount requested).
     */
    struct allocation_result {
        std::byte *ptr;
        std::size_t bytes;
    };


}
