#include <felspar/memory/raw_storage.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("raw_storage");


    struct counters {
        std::size_t construct = {}, destruct = {};
    } count;
    struct counted {
        counted() { ++count.construct; }
        ~counted() { ++count.destruct; }
    };


    auto const cons = suite.test("construct & destroy", [](auto check) {
        count = {};
        felspar::memory::raw_storage<counted> c;
        check(count.construct) == 0u;
        check(count.destruct) == 0u;
        c.destroy_if(false);
        check(count.construct) == 0u;
        check(count.destruct) == 0u;
        c.emplace();
        check(count.construct) == 1u;
        check(count.destruct) == 0u;
        c.destroy_if(true);
        check(count.construct) == 1u;
        check(count.destruct) == 1u;
    });


}
