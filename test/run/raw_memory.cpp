#include <felspar/memory/raw_memory.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("raw_storage");


    struct counters {
        std::size_t construct = {}, assign = {}, moved = {}, copied = {},
                    destruct = {};
    } count;
    struct counted {
        counted() { ++count.construct; }
        counted(counted const &) {
            ++count.construct;
            ++count.copied;
        }
        counted(counted &&) {
            ++count.construct;
            ++count.moved;
        }
        ~counted() { ++count.destruct; }

        counted &operator=(counted const &) {
            ++count.assign;
            return *this;
        }
        counted &operator=(counted &&) {
            ++count.moved;
            return *this;
        }
    };


    auto const cons = suite.test("construct & destroy", [](auto check) {
        count = {};
        felspar::memory::raw_memory<counted> c;
        check(count.construct) == 0u;
        check(count.copied) == 0u;
        check(count.moved) == 0u;
        check(count.assign) == 0u;
        check(count.destruct) == 0u;

        c.destroy_if(false);
        check(count.construct) == 0u;
        check(count.copied) == 0u;
        check(count.moved) == 0u;
        check(count.assign) == 0u;
        check(count.destruct) == 0u;

        c.emplace();
        check(count.construct) == 1u;
        check(count.copied) == 0u;
        check(count.moved) == 0u;
        check(count.assign) == 0u;
        check(count.destruct) == 0u;

        c.destroy_if(true);
        check(count.construct) == 1u;
        check(count.copied) == 0u;
        check(count.moved) == 0u;
        check(count.assign) == 0u;
        check(count.destruct) == 1u;
    });


    auto const assign = suite.test("assignment", [](auto check) {
        count = {};
        felspar::memory::raw_memory<counted> c;
        check(count.construct) == 0u;
        check(count.copied) == 0u;
        check(count.moved) == 0u;
        check(count.assign) == 0u;
        check(count.destruct) == 0u;

        c.assign_or_emplace(false, {});
        check(count.construct) == 3u;
        check(count.copied) == 0u;
        check(count.moved) == 2u;
        check(count.assign) == 0u;
        check(count.destruct) == 2u;

        c.assign_or_emplace(true, {});
        check(count.construct) == 4u;
        check(count.copied) == 0u;
        check(count.moved) == 3u;
        check(count.assign) == 0u;
        check(count.destruct) == 3u;

        c.destroy_if(true);
        check(count.construct) == 4u;
        check(count.copied) == 0u;
        check(count.moved) == 3u;
        check(count.assign) == 0u;
        check(count.destruct) == 4u;
    });


}
