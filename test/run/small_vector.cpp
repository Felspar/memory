#include <felspar/memory/small_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("small_vector");


    auto const meta = suite.test("meta", [](auto check) {
        felspar::memory::small_vector<int> c_int;
        check(c_int.capacity()) == 32;
        felspar::memory::small_vector<std::array<std::byte, 58>, 128> c_56;
        check(c_56.capacity()) == 128;
    });


}
