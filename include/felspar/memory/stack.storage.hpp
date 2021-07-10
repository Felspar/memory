#pragma once


#include <felspar/exceptions/bad_alloc.hpp>
#include <felspar/memory/small_vector.hpp>

#include <array>
#include <stdexcept>
#include <span>


namespace felspar::memory {


    template<std::size_t S = 1u << 20, std::size_t A = 32u, std::size_t CA = 8u>
    class stack_storage {
        /// Storage memory for the
        std::array<std::byte, S> storage alignas(CA);
        using allocation = std::span<std::byte>;
        small_vector<allocation, A> allocations = {};
        allocation available = {storage.data(), storage.size()};

      public:
        /// The size of the usable storage in bytes
        static std::size_t constexpr storage_bytes{S};
        static std::size_t constexpr allocation_count{A};
        static std::size_t constexpr alignment_size{CA};

        stack_storage() noexcept = default;
        stack_storage(stack_storage const &) = delete;
        stack_storage(stack_storage &&) = delete;

        stack_storage &operator=(stack_storage const &) = delete;
        stack_storage &operator=(stack_storage &&) = delete;

        /// Return the amount of free memory remaining in the storage
        auto free() const noexcept { return available.size(); }

        /// Allocate a number of bytes
        std::byte *allocate(std::size_t bytes) {
            bytes = block_size(bytes, alignment_size);
            if (bytes > available.size()) [[unlikely]] {
                throw felspar::stdexcept::bad_alloc{"Out of free memory"};
            } else if (allocations.size() == allocations.capacity())
                    [[unlikely]] {
                throw felspar::stdexcept::bad_alloc{
                        "Out of allocation bookkeeping slots"};
            } else {
                auto alloc = available.first(bytes);
                allocations.push_back(alloc);
                available = available.subspan(bytes);
                return alloc.data();
            }
        }
    };


}
