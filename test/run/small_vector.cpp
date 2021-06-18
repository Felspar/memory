#include <felspar/memory/small_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("small_vector");


    auto const meta = suite.test("meta", [](auto check) {
        felspar::memory::small_vector<int> empty;

        check(empty.capacity()) == 32;
    });


}
