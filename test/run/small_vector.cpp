#include <felspar/memory/small_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("small_vector");


    felspar::memory::small_vector<int> constexpr c_int = {};
    felspar::memory::small_vector<std::array<std::byte, 58>, 128> constexpr c_56;


    auto const meta = suite.test("meta", [](auto check) {
        check(c_int.capacity()) == 32;
        check(c_56.capacity()) == 128;
    });


}
