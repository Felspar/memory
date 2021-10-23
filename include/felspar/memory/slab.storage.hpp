#pragma once


#include <felspar/memory/sizes.hpp>

#include <array>


namespace felspar::memory {


    /**
     * ## Slab storage
     */
    template<std::size_t S = 16u << 10, std::size_t CA = 8u>
    class slab_storage {
        std::array<std::byte, S> storage alignas(CA);
        std::size_t allocated_bytes = {};

      public:
        static std::size_t constexpr storage_bytes{S};
        static std::size_t constexpr alignment_size{CA};

        constexpr slab_storage() noexcept = default;
        slab_storage(slab_storage const &) = delete;
        slab_storage(slab_storage &&) = delete;

        slab_storage &operator=(slab_storage const &) = delete;
        slab_storage &operator=(slab_storage &&) = delete;

        /// Return the approximate number of free bytes
        [[nodiscard]] constexpr std::size_t free() const noexcept {
            return storage.size() - allocated_bytes;
        }

        [[nodiscard]] constexpr std::byte *allocate(std::size_t bytes) {
            if (bytes > free()) {
                throw std::bad_alloc{};
            } else {
                std::byte *base = storage.data() + allocated_bytes;
                allocated_bytes += aligned_offset(bytes, alignment_size);
                return base;
            }
        }
        constexpr void deallocate(void *) {}
    };


}
