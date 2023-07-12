#include <felspar/memory/shared_buffer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("shared_buffer");


    auto const construct = suite.test(
            "construct",
            [](auto check) {
                auto bytes =
                        felspar::memory::shared_buffer<std::byte>::allocate(
                                1024);
                check(bytes.empty()) == false;
                check(bytes.size()) == 1024u;

                auto strings =
                        felspar::memory::shared_buffer<std::string>::allocate(
                                20, "hello");
                check(strings.empty()) == false;
                check(strings.size()) == 20u;
                check(strings[0]) == "hello";

                auto const cstrings{strings};
                check(cstrings.empty()) == false;
                check(cstrings.size()) == 20u;
                check(cstrings[0]) == "hello";
                check(cstrings.control_block()) == strings.control_block();

                strings[1] = "world";
                check(cstrings[1]) == "world";
            },
            [](auto check) {
                std::vector v{1, 2, 3};
                auto buffer =
                        felspar::memory::shared_buffer<int>::wrap(std::move(v));
                check(buffer.size()) == 3u;
                check(buffer.at(0)) == 1;
                check(buffer.at(1)) == 2;
                check(buffer.at(2)) == 3;
            });


    auto const ds = suite.test("access", [](auto check) {
        auto strings = felspar::memory::shared_buffer<std::string>::allocate(
                20, "hello");
        auto const &cstrings = strings;

        check(strings.data()) == cstrings.data();
        check(strings.size()) == cstrings.size();
    });


}
