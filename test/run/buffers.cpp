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

        felspar::memory::accumulation_buffer<std::byte> acc{10};
        check(acc.empty()) == true;
        check(acc.size()) == 0u;
    });


    auto const aa = suite.test("allocate/accumulation_buffer", [](auto check) {
        felspar::memory::accumulation_buffer<std::byte> bytes{1024};
        check(bytes.size()) == 0u;
        bytes.ensure_length(1024);
        check(bytes.size()) == 1024u;
        check(bytes[0]) == std::byte{};

        felspar::memory::accumulation_buffer<std::string> strings{30};
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

        check(first.control_block()) == second.control_block();
    });


}
