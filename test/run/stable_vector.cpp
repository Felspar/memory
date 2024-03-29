#include <felspar/memory/stable_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("stable_vector");


    auto const construction = suite.test("construction", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv1, sv2(20, 123);
        check(sv1.empty()) == true;
        check(sv2.empty()) == false;
        check(sv1.size()) == 0u;
        check(sv2.size()) == 20u;
        check(sv1.capacity()) == 0u;
        check(sv2.capacity()) == 24u;
        for (std::size_t index{}; index < 20; ++index) {
            check(sv2[index]) == 123;
        }
    });


    auto const mutation = suite.test("mutation", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv;
        for (int index{}; index < 20; ++index) {
            sv.push_back(index);
            check(sv[index]) == index;
        }
        check(sv.capacity()) == 24u;
    });


    auto const clear = suite.test("clear", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv(20, 123);
        sv.clear();
        check(sv.capacity()) == 24u;
        check(sv.empty()) == true;
        for (int index{}; index < 10; ++index) {
            sv.push_back(index);
            check(sv[index]) == index;
        }
        check(sv.size()) == 10u;
        check(sv.capacity()) == 24u;
    });


    auto const reserve = suite.test("reserve", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv;
        sv.reserve(0);
        check(sv.capacity()) == 0u;
        sv.reserve(1);
        check(sv.capacity()) == 8u;
        sv.reserve(20);
        check(sv.capacity()) == 24u;
    });


}
