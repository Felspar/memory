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
        small_vector<allocation, A + 1> allocations = {
                allocation{storage.data(), storage.size()}};

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
        [[nodiscard]] auto free() const noexcept {
            return allocations.back().size();
        }

        /// Allocate a number of bytes
        [[nodiscard]] std::byte *allocate(
                std::size_t bytes,
                source_location loc = source_location::current()) {
            bytes = block_size(bytes, alignment_size);
            if (bytes > allocations.back().size()) [[unlikely]] {
                throw felspar::stdexcept::bad_alloc{
                        "Out of free memory", std::move(loc)};
            } else if (allocations.size() == allocations.capacity())
                    [[unlikely]] {
                throw felspar::stdexcept::bad_alloc{
                        "Out of allocation bookkeeping slots", std::move(loc)};
            } else {
                auto &alloc = allocations.back();
                allocations.push_back(alloc.subspan(bytes));
                alloc = alloc.first(bytes);
                return alloc.data();
            }
        }
        void deallocate(
                std::byte *location,
                std::size_t count = 1u,
                source_location loc = source_location::current()) {
            if (count != 1u) {
                throw felspar::stdexcept::runtime_error{
                        "Deallocation count must be 1", std::move(loc)};
            }
            for (auto pos = allocations.begin(); pos != allocations.end();
                 ++pos) {
                if (location >= pos->data()
                    and location < pos->data() + pos->size()) {
                    auto const freeing = *pos;
                    allocations.erase(pos);
                    *pos = {freeing.data(), freeing.size() + pos->size()};
                    return;
                }
            }
            throw felspar::stdexcept::runtime_error{
                    "Bookkeeping entry for allocation not found",
                    std::move(loc)};
        }
    };


}
