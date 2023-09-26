#include <felspar/memory/stable_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("stable_vector");


    auto const c = suite.test("construction", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv1, sv2(20, 123);
        check(sv1.empty()) == true;
        check(sv2.size()) == 20u;
        for (std::size_t index{}; index < 20; ++index) {
            check(sv2[index]) == 123;
        }
    });


    auto const a = suite.test("mutation", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv;
        for (int index{}; index < 20; ++index) {
            sv.push_back(index);
            check(sv[index]) == index;
        }
    });


    auto const e = suite.test("clear", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv(20, 123);
        sv.clear();
        check(sv.empty()) == true;
        for (int index{}; index < 10; ++index) {
            sv.push_back(index);
            check(sv[index]) == index;
        }
        check(sv.size()) == 10u;
    });


}