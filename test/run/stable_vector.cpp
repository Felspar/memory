#include <felspar/memory/stable_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("stable_vector");


    auto const c = suite.test("construction", [](auto check) {
        felspar::memory::stable_vector<int, 8> sv1, sv2(20);
        check(sv1.empty()) == true;
        check(sv2.size()) == 20u;
    });


}
