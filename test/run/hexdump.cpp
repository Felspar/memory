#include <felspar/memory/hexdump.hpp>
#include <felspar/test.hpp>

#include <array>


namespace {


    auto const suite = felspar::testsuite(
            "hexdump",
            [](auto check) {
                std::stringstream ss;
                std::array<std::byte, 0> a{};
                felspar::memory::hexdump(ss, a);
                check(ss.str()) == "Showing 0 bytes out of 0\n";
            },
            [](auto check) {
                std::stringstream ss;
                std::array<std::byte, 16> a{std::byte{'a'}};
                felspar::memory::hexdump(ss, a);
                check(ss.str()) ==
                        "Showing 16 bytes out of 16\n"
                        "| 61 00 00 00 00 00 00 00 | 00 00 00 00 00 00 00 00 |   |a°°°°°°°|°°°°°°°°|\n";
            },
            [](auto check) {
                std::stringstream ss;
                std::array<std::byte, 16> a{std::byte{'a'}};
                ss << felspar::memory::hexdump(a);
                check(ss.str()) ==
                        "Showing 16 bytes out of 16\n"
                        "| 61 00 00 00 00 00 00 00 | 00 00 00 00 00 00 00 00 |   |a°°°°°°°|°°°°°°°°|\n";
            },
            [](auto check) {
                std::stringstream ss;
                std::array<char, 16> a{'a', 'b', 'c'};
                ss << felspar::memory::hexdump(a);
                check(ss.str()) ==
                        "Showing 16 bytes out of 16\n"
                        "| 61 62 63 00 00 00 00 00 | 00 00 00 00 00 00 00 00 |   |abc°°°°°|°°°°°°°°|\n";
            },
            [](auto check) {
                std::array<char, 16> a{'a', 'b', 'c'};
                std::string hd = felspar::memory::hexdump(a, 8);
                check(hd) ==
                        "Showing 8 bytes out of 16\n"
                        "| 61 62 63 00 00 00 00 00 |                         |   |abc°°°°°|        |\n";
            });


}
