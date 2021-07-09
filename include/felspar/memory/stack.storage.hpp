#pragma once


#include <felspar/memory/sizes.hpp>

#include <array>
#include <span>


namespace felspar::memory {


    template<std::size_t S = 1u << 20, std::size_t A = 32u, std::size_t CA = 8u>
    class stack_storage {
        /// Storage memory for the
        std::array<std::byte, S> storage alignas(CA);
        using allocation = std::span<std::byte>;
        std::array<allocation, A> allocations = {};
        allocation free = {storage.data(), storage.size()};

      public:
        /// The size of the usable storage in bytes
        static std::size_t constexpr storage_bytes{S};
        static std::size_t constexpr allocation_count{A};
        static std::size_t constexpr alignment_size{CA};

        stack_storage();
        stack_storage(stack_storage const &) = delete;
        stack_storage(stack_storage &&) = delete;

        stack_storage &operator=(stack_storage const &) = delete;
        stack_storage &operator=(stack_storage &&) = delete;
    };


}
