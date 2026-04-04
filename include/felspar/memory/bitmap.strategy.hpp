#pragma once


#include <felspar/memory/concepts.hpp>
#include <felspar/memory/exceptions.hpp>

#include <cstddef>
#include <source_location>


namespace felspar::memory::bitmap {


    /**
     * ## Bitmap allocation
     *
     * The bitmap allocator uses a bitmap to store which blocks are free and
     * which are currently in use. A set bit indicates a used location and a
     * zero bit indicates allocated. All memory requests must be for less than
     * the block size.
     */


    /// ### The number of bits available in the type
    template<typename BM>
    constexpr std::size_t bitcount = sizeof(BM) * 8;


    /// ### Return the memory location that is allocated
    template<typename BM>
    constexpr inline std::size_t nextbit(BM allocations) {
        std::size_t bit{};
        for (BM mask{1}; bit < bitcount<BM> and allocations & mask;
             ++bit, mask = (mask << 1u)) {}
        return bit;
    }


    template<typename BM>
    std::byte *allocate(BM &bm, std::byte *base, std::size_t const blocksize) {
        if (auto bit = nextbit(bm); bit < bitcount<BM>) {
            bm |= 1 << bit;
            return base + blocksize * bit;
        } else {
            return nullptr;
        }
    }


    template<typename BM>
    inline void deallocate(
            std::byte *ptr,
            BM &bm,
            std::byte *base,
            std::size_t const blocksize) {
        std::size_t const bit = (ptr - base) / blocksize;
        bm &= ~(1 << bit);
    }


}


namespace felspar::memory {


    /**
     * ## Bitmap allocator strategy
     *
     * A fixed-block pool allocator using a bitmap to track allocations.
     * All allocations must be for the same block size. Provides nullable
     * and ownership semantics for composition as well as over allocation.
     */
    template<typename BM>
    class bitmap_strategy final {
        BM m_bitmap{};
        std::byte *m_base;
        std::size_t m_blocksize;


      public:
        /// ### Construct with a base pointer and block size
        bitmap_strategy(
                std::byte *const base, std::size_t const blocksize) noexcept
        : m_base{base}, m_blocksize{blocksize} {}

        /// #### Non-copyable and non-movable for safety
        bitmap_strategy(bitmap_strategy const &) = delete;
        bitmap_strategy(bitmap_strategy &&) = delete;
        bitmap_strategy &operator=(bitmap_strategy const &) = delete;
        bitmap_strategy &operator=(bitmap_strategy &&) = delete;


        /// ### Nullable allocation
        [[nodiscard]] std::byte *try_allocate(std::size_t const bytes) noexcept {
            return try_allocate_at_least(bytes).ptr;
        }
        [[nodiscard]] allocation_result
                try_allocate_at_least(std::size_t const bytes) noexcept {
            if (bytes > m_blocksize) { return {nullptr, {}}; }
            return {bitmap::allocate(m_bitmap, m_base, m_blocksize),
                    m_blocksize};
        }


        /// ### Throwing allocation
        [[nodiscard]] std::byte *allocate(
                std::size_t const bytes,
                std::source_location const loc =
                        std::source_location::current()) {
            return allocate_at_least(bytes, loc).ptr;
        }


        /// ### Allocate at least `bytes`, reporting the true block size
        [[nodiscard]] allocation_result allocate_at_least(
                std::size_t const bytes,
                std::source_location const loc = std::source_location::current())
        /**
         * Because every allocation consumes a full block of `m_blocksize`
         * bytes, the caller receives exactly `m_blocksize` bytes regardless
         * of the amount requested.
         */
        {
            auto *const ptr = try_allocate(bytes);
            if (ptr == nullptr) {
                detail::throw_bad_alloc(
                        "bitmap_strategy pool exhausted or oversized allocation",
                        loc);
            }
            return {ptr, m_blocksize};
        }


        /// ### Deallocate a previously allocated block
        void deallocate(void *const ptr, std::size_t) noexcept {
            bitmap::deallocate(
                    static_cast<std::byte *>(ptr), m_bitmap, m_base,
                    m_blocksize);
        }


        /// ### Check if a pointer was allocated from this strategy
        [[nodiscard]] bool owns(void const *const ptr) const noexcept {
            auto const *bptr = static_cast<std::byte const *>(ptr);
            return bptr >= m_base
                    and bptr < m_base + m_blocksize * bitmap::bitcount<BM>;
        }


        /// ### Return the total capacity in blocks
        constexpr std::size_t capacity() const noexcept {
            return bitmap::bitcount<BM>;
        }
    };


}
