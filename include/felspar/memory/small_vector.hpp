#pragma once


#include <array>


namespace felspar::memory {


    namespace detail {


        /// Calculate how big a space a type will need in a contiguous memory
        /// area taking into account its size and alignment
        constexpr std::size_t
                block_size(std::size_t const size, std::size_t const align) {
            return (align * ((size + align - 1) / align));
        }


    }


    /// Stack allocated dynamic vector
    template<typename T, std::size_t N = 32>
    class small_vector final {
        static std::size_t constexpr block_size =
                detail::block_size(sizeof(T), alignof(T));

        std::size_t entries = {};
        std::array<std::byte, block_size * N> storage alignas(T);

      public:
        using value_type = T;

        /// Constructors
        small_vector(){};

        /// Capacity and meta-data
        auto capacity() const { return N; }
    };


}
