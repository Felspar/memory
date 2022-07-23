#include <felspar/memory/hexdump.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite(
            "hexdump",
            [](auto check) {
                std::stringstream ss;
                std::array<std::byte, 0> a{};
                felspar::memory::hexdump(ss, a);
                check(ss.str()) == "0 bytes\n";
            },
            [](auto check) {
                std::stringstream ss;
                std::array<std::byte, 16> a{std::byte{'a'}};
                felspar::memory::hexdump(ss, a);
                check(ss.str()) == "16 bytes\n"
                    "| 61 00 00 00 00 00 00 00 | 00 00 00 00 00 00 00 00 |   |a°°°°°°°|°°°°°°°°|\n";
            });


}
