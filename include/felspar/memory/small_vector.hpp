#pragma once


#include <array>


namespace felspar::memory {


    /// Stack allocated dynamic vector
    template<typename T, std::size_t N = 32>
    class small_vector final {
        static std::size_t constexpr block_size = sizeof(T);

        std::size_t entries = {};
        std::array<std::byte, block_size * N> storage alignas(T);

      public:
        using value_type = T;

        /// Constructors
        small_vector() = default;

        /// Capacity and meta-data
        auto capacity() const { return N; }
    };


}
