#include <felspar/memory/small_ring.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("small_ring");


    auto const c = suite.test("construct", [](auto check) {
        felspar::memory::small_ring<int, 5> s5;
        check(s5.empty()) == true;
        check(s5.size()) == 0u;
    });


    auto const i = suite.test("int", [](auto check) {
        felspar::memory::small_ring<int, 3> s3;

        s3.push(1);
        check(s3.empty()) == false;
        check(s3.size()) == 1u;
        check(s3.front()) == 1;
        check(s3.back()) == 1;

        s3.push(2);
        check(s3.empty()) == false;
        check(s3.size()) == 2u;
        check(s3.front()) == 2;
        check(s3.back()) == 1;

        s3.push(3);
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(s3.front()) == 3;
        check(s3.back()) == 1;

        s3.push(4);
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(s3.front()) == 4;
        check(s3.back()) == 2;

        s3.push(5);
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(s3.front()) == 5;
        check(s3.back()) == 3;

        s3.push(6);
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(s3.front()) == 6;
        check(s3.back()) == 4;

        s3.push(7);
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(s3.front()) == 7;
        check(s3.back()) == 5;

        s3.pop_back();
        check(s3.empty()) == false;
        check(s3.size()) == 2u;
        check(s3.front()) == 7;
        check(s3.back()) == 6;

        s3.pop_back();
        check(s3.empty()) == false;
        check(s3.size()) == 1u;
        check(s3.front()) == 7;
        check(s3.back()) == 7;

        s3.pop_back();
        check(s3.empty()) == true;
        check(s3.size()) == 0u;
    });


    auto const u = suite.test("std::unique_ptr", [](auto check) {
        felspar::memory::small_ring<std::unique_ptr<int>, 3> s3;

        s3.push(std::make_unique<int>(1));
        check(s3.empty()) == false;
        check(s3.size()) == 1u;
        check(*s3.front()) == 1;
        check(*s3.back()) == 1;

        s3.push(std::make_unique<int>(2));
        check(s3.empty()) == false;
        check(s3.size()) == 2u;
        check(*s3.front()) == 2;
        check(*s3.back()) == 1;

        s3.push(std::make_unique<int>(3));
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(*s3.front()) == 3;
        check(*s3.back()) == 1;

        s3.push(std::make_unique<int>(4));
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(*s3.front()) == 4;
        check(*s3.back()) == 2;

        s3.push(std::make_unique<int>(5));
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(*s3.front()) == 5;
        check(*s3.back()) == 3;

        s3.push(std::make_unique<int>(6));
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(*s3.front()) == 6;
        check(*s3.back()) == 4;

        s3.push(std::make_unique<int>(7));
        check(s3.empty()) == false;
        check(s3.size()) == 3u;
        check(*s3.front()) == 7;
        check(*s3.back()) == 5;

        s3.pop_back();
        check(s3.empty()) == false;
        check(s3.size()) == 2u;
        check(*s3.front()) == 7;
        check(*s3.back()) == 6;

        s3.pop_back();
        check(s3.empty()) == false;
        check(s3.size()) == 1u;
        check(*s3.front()) == 7;
        check(*s3.back()) == 7;

        s3.pop_back();
        check(s3.empty()) == true;
        check(s3.size()) == 0u;
    });


}
