#pragma once


#include <utility>


namespace felspar::memory::bitmap {


    /**
     * The bitmap allocator uses a bitmap to store which blocks are free and
     * which are currently in use. A set bit indicates a used location and a
     * zero bit indicates allocated. All memory requests must be for less than
     * the block size.
     */


    /// The number of bits available in the type
    template<typename BM>
    constexpr std::size_t bitcount = sizeof(BM) * 8;


    /// Return the memory location that is allocated
    template<typename BM>
    constexpr inline std::size_t nextbit(BM allocations) {
        std::size_t bit{};
        for (BM mask{1}; bit < bitcount<BM> and allocations & mask;
             ++bit, mask = (mask << 1u)) {}
        return bit;
    }


    template<typename BM>
    std::byte *allocate(BM &bm, std::byte *base, std::size_t blocksize) {
        if (auto bit = nextbit(bm); bit < bitcount<BM>) {
            bm |= 1 << bit;
            return base + blocksize * bit;
        } else {
            return nullptr;
        }
    }


    template<typename BM>
    inline void free(
            std::byte *ptr, BM &bm, std::byte *base, std::size_t blocksize) {
        std::size_t const bit = (ptr - base) / blocksize;
        bm &= ~(1 << bit);
    }


}
