#include <felspar/memory/bitmap.strategy.hpp>
#include <felspar/test.hpp>

#include <array>
#include <cstdint>


namespace {


    auto const suite = felspar::testsuite("bitmap allocator");


    auto const nb = suite.test("nextbit", [](auto check) {
        namespace bm = felspar::memory::bitmap;
        check(bm::nextbit(std::uint8_t{0b0000'0000})) == 0u;
        check(bm::nextbit(std::uint8_t{0b0000'0111})) == 3u;
        check(bm::nextbit(std::uint8_t{0b1111'1110})) == 0u;
        check(bm::nextbit(std::uint8_t{0b1111'0111})) == 3u;
        check(bm::nextbit(std::uint8_t{0b1111'1111})) == 8u;
    });


    auto const al = suite.test("allocate", [](auto check) {
        std::array<std::byte, 32> memory{};
        std::uint32_t blocks{};

        namespace bm = felspar::memory::bitmap;

        check(bm::allocate(blocks, memory.data(), 1u)) == memory.data();
        check(blocks) == 1u;

        check(bm::allocate(blocks, memory.data(), 1u)) == memory.data() + 1;
        check(blocks) == 3u;

        bm::deallocate(memory.data(), blocks, memory.data(), 1u);
        check(blocks) == 2u;

        bm::deallocate(memory.data() + 1, blocks, memory.data(), 1u);
        check(blocks) == 0u;
    });


}
