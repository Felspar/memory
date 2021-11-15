#pragma once


#include <felspar/exceptions/bad_alloc.hpp>
#include <felspar/memory/small_vector.hpp>

#include <array>
#include <stdexcept>
#include <span>


namespace felspar::memory {


    /**
     * ## Stack based allocator
     *
     * An allocator intended for use with small allocations where their
     * lifetimes don't extend beyond the allocator.
     *
     * This allocator is not thread safe.
     */
    template<
            std::size_t S = 2u << 10,
            std::size_t A = 16u,
            std::size_t CA = alignof(std::max_align_t)>
    class stack_storage {
        /// Storage memory for the
        std::array<std::byte, S> storage alignas(CA);
        using allocation = std::span<std::byte>;
        small_vector<allocation, A> allocations = {},
                                    available = {allocation{
                                            storage.data(), storage.size()}};

      public:
        /// The size of the usable storage in bytes
        static std::size_t constexpr storage_bytes{S};
        static std::size_t constexpr allocation_count{A};
        static std::size_t constexpr alignment_size{CA};

        constexpr stack_storage() noexcept = default;
        stack_storage(stack_storage const &) = delete;
        stack_storage(stack_storage &&) = delete;

        stack_storage &operator=(stack_storage const &) = delete;
        stack_storage &operator=(stack_storage &&) = delete;

        /// Return the amount of free memory remaining in the storage
        [[nodiscard]] constexpr auto free() const noexcept {
            std::size_t f{};
            for (auto a : available) { f += a.size(); }
            return f;
        }

        /// Allocate a number of bytes
        [[nodiscard]] constexpr std::byte *allocate(
                std::size_t const abytes,
                source_location loc = source_location::current()) {
            auto const bytes = block_size(abytes, alignment_size);
            /// Search through available blocks for the smallest that satisfies
            /// the allocation
            std::size_t best_alloc = -1;
            auto best_pos = available.end();
            for (auto pos = available.begin(); pos != available.end(); ++pos) {
                if (pos->size() >= bytes and pos->size() < best_alloc) {
                    best_alloc = pos->size();
                    best_pos = pos;
                }
            }
            if (best_pos == available.end()) /*[[unlikely]]*/ {
                if (bytes > free()) {
                    throw felspar::stdexcept::bad_alloc{
                            "Out of free memory -- memory exhausted",
                            std::move(loc)};
                } else {
                    throw felspar::stdexcept::bad_alloc{
                            "Out of free memory -- memory fragmentation",
                            std::move(loc)};
                }
            }

            /// Do the bookkeeping needed to record the allocation and remove
            /// from the available memory
            if (allocations.size() == allocations.capacity()) /*[[unlikely]]*/ {
                throw felspar::stdexcept::bad_alloc{
                        "Out of allocation bookkeeping slots", std::move(loc)};
            } else {
                auto const alloc = best_pos->first(bytes);
                if (auto const remaining = best_pos->subspan(bytes);
                    remaining.size() >= alignment_size) {
                    *best_pos = remaining;
                } else {
                    available.erase(best_pos);
                }
                allocations.push_back(alloc);
                return alloc.data();
            }
        }
        constexpr void deallocate(
                void *location,
                source_location loc = source_location::current()) {
            for (auto pos = allocations.begin(); pos != allocations.end();
                 ++pos) {
                if (location == pos->data()) {
                    for (auto &free : available) {
                        if (pos->data() + pos->size() == free.data()) {
                            free = {pos->data(), pos->size() + free.size()};
                            allocations.erase(pos);
                            return;
                        } else if (free.data() + free.size() == pos->data()) {
                            free = {free.data(), free.size() + pos->size()};
                            allocations.erase(pos);
                            return;
                        }
                    }
                    available.emplace_back(pos->data(), pos->size());
                    allocations.erase(pos);
                    return;
                }
            }
            throw felspar::stdexcept::runtime_error{
                    "Bookkeeping entry for allocation not found",
                    std::move(loc)};
        }
    };


}
