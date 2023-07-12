#include <felspar/memory/shared_buffer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("shared_buffer");


    auto const construct = suite.test("construct", [](auto check) {
        std::vector v{1, 2, 3};
        auto buffer = felspar::memory::shared_buffer<int>::wrap(std::move(v));
        check(buffer.size()) == 3u;
        check(buffer.at(0)) == 1;
        check(buffer.at(1)) == 2;
        check(buffer.at(2)) == 3;
    });


}
