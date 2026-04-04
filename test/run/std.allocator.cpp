#include <felspar/memory/std.allocator.hpp>
#include <felspar/memory/stack.storage.hpp>
#include <felspar/test.hpp>

#include <vector>


namespace {


    auto const suite = felspar::testsuite("std.allocator");


    auto const basic_vector_usage =
            suite.test("basic vector usage with stack_storage", [](auto check) {
                felspar::memory::stack_storage<1024> storage;
                felspar::memory::allocator<int, decltype(storage)> alloc{
                        storage};

                std::vector<int, decltype(alloc)> vec{alloc};
                check(vec.empty());

                // Add enough elements to trigger growth and reallocation
                for (int i = 0; i < 50; ++i) { vec.push_back(i); }

                check(vec.size()) == 50u;
                // Verify the values are correct
                for (int i = 0; i < 50; ++i) { check(vec[i]) == i; }

                // Verify we can iterate
                int expected = 0;
                for (auto const &v : vec) { check(v) == expected++; }
            });


    auto const equality = suite.test(
            "equality comparison",
            [](auto check) {
                felspar::memory::stack_storage<1024> storage;
                felspar::memory::allocator<int, decltype(storage)> alloc1{
                        storage};
                felspar::memory::allocator<int, decltype(storage)> alloc2{
                        storage};

                check(alloc1 == alloc2);
                check(not(alloc1 != alloc2));
            },
            [](auto check) {
                felspar::memory::stack_storage<1024> storage1;
                felspar::memory::stack_storage<1024> storage2;
                felspar::memory::allocator<int, decltype(storage1)> alloc1{
                        storage1};
                felspar::memory::allocator<int, decltype(storage2)> alloc2{
                        storage2};

                check(alloc1 != alloc2);
                check(not(alloc1 == alloc2));
            });


    auto const rebind = suite.test("rebind", [](auto check) {
        using AllocInt = felspar::memory::allocator<
                int, felspar::memory::stack_storage<1024>>;
        using AllocDouble = felspar::memory::allocator<
                double, felspar::memory::stack_storage<1024>>;

        felspar::memory::stack_storage<1024> storage;
        AllocInt alloc_int{storage};

        // Rebind via converting constructor
        AllocDouble alloc_double = alloc_int;
        check(alloc_double == alloc_double);

        // Rebind via rebind typedef
        using ReboundDouble = typename AllocInt::template rebind<double>::other;
        static_assert(std::is_same_v<ReboundDouble, AllocDouble>);
        ReboundDouble alloc_rebound{alloc_int};
        check(alloc_rebound == alloc_double);
    });


    auto const ma = suite.test("multiple allocators", [](auto check) {
        felspar::memory::stack_storage<4096> storage;
        felspar::memory::allocator<int, decltype(storage)> alloc{storage};

        std::vector<int, decltype(alloc)> vec1{alloc};
        std::vector<int, decltype(alloc)> vec2{alloc};

        for (int i = 0; i < 20; ++i) {
            vec1.push_back(i);
            vec2.push_back(100 + i);
        }

        check(vec1.size()) == 20u;
        check(vec2.size()) == 20u;

        for (int i = 0; i < 20; ++i) {
            check(vec1[i]) == i;
            check(vec2[i]) == (100 + i);
        }
    });


}
