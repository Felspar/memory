#include <felspar/memory/small_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("detail");


    auto const bs = suite.test("block_size", [](auto check) {
        /// Whole multiples
        check(felspar::memory::detail::block_size(8, 8)) == 8;
        check(felspar::memory::detail::block_size(16, 8)) == 16;
        /// Over-aligned
        check(felspar::memory::detail::block_size(2, 8)) == 8;
        check(felspar::memory::detail::block_size(4, 8)) == 8;
        check(felspar::memory::detail::block_size(10, 8)) == 16;
        check(felspar::memory::detail::block_size(58, 8)) == 64;
    });


}
