#pragma once


#include <cstddef>


namespace felspar::memory {


    /// Calculate how big a space a type will need in a contiguous memory area
    /// taking into account its size and alignment
    constexpr inline std::size_t block_size(
            std::size_t const size, std::size_t const align) noexcept {
        return (align * ((size + align - 1) / align));
    }


}
