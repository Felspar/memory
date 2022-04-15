#include <felspar/memory/accumulation_buffer.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("buffers");


    auto const c = suite.test("construct", [](auto check) {
        felspar::memory::shared_buffer<std::byte> bytes{};
        check(bytes.empty()) == true;
        check(bytes.size()) == 0u;
        check(bytes.control_block()) == nullptr;

        felspar::memory::shared_buffer_view view{bytes};
        check(view.empty()) == true;
        check(view.size()) == 0u;
        check(view.control_block()) == nullptr;

        felspar::memory::shared_buffer_view<std::byte const> cview{bytes};
        check(cview.empty()) == true;
        check(cview.size()) == 0u;
        check(cview.control_block()) == nullptr;

        felspar::memory::accumulation_buffer<std::byte> acc;
        check(acc.empty()) == true;
        check(acc.size()) == 0u;
    });


    auto const ab = suite.test("allocate/shared_buffer", [](auto check) {
        auto bytes = felspar::memory::shared_buffer<std::byte>::allocate(1024);
        check(bytes.empty()) == false;
        check(bytes.size()) == 1024u;

        auto strings = felspar::memory::shared_buffer<std::string>::allocate(
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
    });
    auto const aa = suite.test("allocate/accumulation_buffer", [](auto check) {
        felspar::memory::accumulation_buffer<std::byte> bytes;
        check(bytes.size()) == 0u;
        bytes.ensure_length(1024);
        check(bytes.size()) == 1024;
        check(bytes[0]) == std::byte{};

        felspar::memory::accumulation_buffer<std::string> strings;
        check(strings.size()) == 0u;
        strings.ensure_length(10, "hello");
        check(strings.size()) == 10u;
        auto const first = strings.first(5);

        strings.ensure_length(10, "world");
        check(strings.size()) == 10u;
        check(strings[4]) == "hello";
        check(strings[5]) == "world";
        auto const second = strings.first(5);
        check(strings.size()) == 5u;
        check(strings[0]) == "world";
    });


}
