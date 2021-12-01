#include <felspar/memory/bitmap.strategy.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("bitmap allocator");


    auto const nb = suite.test("nextbit", [](auto check) {
        check(felspar::memory::bitmap::nextbit(std::uint8_t{0b0000'0000}))
                == 0u;
        check(felspar::memory::bitmap::nextbit(std::uint8_t{0b0000'0111}))
                == 3u;
        check(felspar::memory::bitmap::nextbit(std::uint8_t{0b1111'1110}))
                == 0u;
        check(felspar::memory::bitmap::nextbit(std::uint8_t{0b1111'0111}))
                == 3u;
        check(felspar::memory::bitmap::nextbit(std::uint8_t{0b1111'1111}))
                == 8u;
    });


}
