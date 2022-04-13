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


    auto const a = suite.test("allocate", [](auto check) {
        auto bytes = felspar::memory::shared_buffer<std::byte>::allocate(1024);
        check(bytes.empty()) == false;
        check(bytes.size()) == 1024u;

        auto strings =
                felspar::memory::shared_buffer<std::string>::allocate(20);
        check(strings.empty()) == false;
        check(strings.size()) == 20u;
    });


}
