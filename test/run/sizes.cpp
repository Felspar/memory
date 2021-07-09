#include <felspar/memory/sizes.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("sizes");


    auto const bs = suite.test("block_size", [](auto check) {
        /// Whole multiples
        check(felspar::memory::block_size(8, 8)) == 8u;
        check(felspar::memory::block_size(16, 8)) == 16u;
        /// Over-aligned
        check(felspar::memory::block_size(2, 8)) == 8u;
        check(felspar::memory::block_size(4, 8)) == 8u;
        check(felspar::memory::block_size(10, 8)) == 16u;
        check(felspar::memory::block_size(58, 8)) == 64u;
    });


}
