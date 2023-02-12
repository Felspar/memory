#pragma once


#include <cstddef>


namespace felspar::memory {


    /// ## Memory block size
    /**
     * Calculate how big a space a type will need in a contiguous memory area.
     *
     * For a given sized memory allocation and an alignment requirement for the
     * following memory, this returns the total size of memory that needs to be
     * allocated to maintain the following alignment.
     */
    constexpr std::size_t block_size(
            std::size_t const size, std::size_t const align) noexcept {
        return (align * ((size + align - 1) / align));
    }


    /// Given a base position, calculate the next one above it at the requested
    /// alignment
    constexpr std::size_t aligned_offset(
            std::size_t const base, std::size_t const alignment) {
        return (base + alignment - 1) & ~(alignment - 1);
    }


}
