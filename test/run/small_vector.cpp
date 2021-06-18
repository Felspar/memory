#include <felspar/memory/small_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("small_vector");


    auto const access = suite.test("accessors", [](auto check) {
        felspar::memory::small_vector<int> const int_c{0, 1, 2, 3, 4};
        check(int_c[0]) == 0;
        check(int_c[1]) == 1;
        check(int_c[2]) == 2;
        check(int_c[3]) == 3;
        felspar::memory::small_vector<int> const int_m{0, 1, 2, 3, 4};
        check(int_m[0]) == 0;
        check(int_m[1]) == 1;
        check(int_m[2]) == 2;
        check(int_m[3]) == 3;
    });

    auto const meta = suite.test("meta", [](auto check) {
        felspar::memory::small_vector<int> c_int;
        check(c_int.capacity()) == 32;
        check(c_int.size()) == 0;
        felspar::memory::small_vector<std::array<std::byte, 58>, 128> c_56;
        check(c_56.capacity()) == 128;
        check(c_56.size()) == 0;
    });


    auto const mod = suite.test("modifiers", [](auto check) {
        felspar::memory::small_vector<int> c_int;
        c_int.push_back(0);
        check(c_int.size()) == 1;
        c_int.push_back(1);
        check(c_int.size()) == 2;
    });


}
