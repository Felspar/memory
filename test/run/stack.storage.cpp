#include <felspar/memory/stack.storage.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("stack.storage");

    auto const ga = suite.test("good alloc", [](auto check) {
        felspar::memory::stack_storage<1u << 10, 4u, 8u> stack;

        auto a1 = stack.allocate(8u);
        check(a1) == reinterpret_cast<std::byte const *>(&stack);
        check(stack.free()) == stack.storage_bytes - 8u;

        auto a2 = stack.allocate(1u);
        check(a2) == reinterpret_cast<std::byte const *>(&stack) + 8u;
        check(stack.free()) == stack.storage_bytes - 16u;

        auto a3 = stack.allocate(stack.storage_bytes - 16u);
        check(a3) == reinterpret_cast<std::byte const *>(&stack) + 16u;
    });


    auto const ba = suite.test("bad alloc", [](auto check) {
        felspar::memory::stack_storage<64, 2, 16> stack;

        check([&]() {
            [[maybe_unused]] auto _ = stack.allocate(65u);
        }).throws(felspar::stdexcept::bad_alloc{"Out of free memory"});

        auto a1 = stack.allocate(1u);
        check(a1) == reinterpret_cast<std::byte const *>(&stack);
        check(stack.free()) == stack.storage_bytes - 16u;

        auto a2 = stack.allocate(1u);
        check(a2) == reinterpret_cast<std::byte const *>(&stack) + 16u;
        check(stack.free()) == stack.storage_bytes - 32u;

        check([&]() { [[maybe_unused]] auto _ = stack.allocate(8u); })
                .throws(felspar::stdexcept::bad_alloc{
                        "Out of allocation bookkeeping slots"});
    });


    auto const d = suite.test("deallocate", [](auto check) {
        felspar::memory::stack_storage<64u, 8u, 8u> stack;

        auto a1 = stack.allocate(1u);
        check(a1) == reinterpret_cast<std::byte const *>(&stack);
        auto a2 = stack.allocate(1u);
        check(a2) == reinterpret_cast<std::byte const *>(&stack) + 8u;
        auto a3 = stack.allocate(1u);
        check(a3) == reinterpret_cast<std::byte const *>(&stack) + 16u;

        stack.deallocate(a3);
        auto a4 = stack.allocate(1u);
        check(a4) == a3;

        stack.deallocate(a1);
        auto a5 = stack.allocate(1u);
        check(a5) == reinterpret_cast<std::byte const *>(&stack) + 24u;

        stack.deallocate(a2);
        stack.deallocate(a4);
        stack.deallocate(a5);
        check(stack.free()) == 64u;

        auto a6 = stack.allocate(1u);
        check(a6) == reinterpret_cast<std::byte const *>(&stack);
        stack.deallocate(a6);
        check(stack.free()) == 64u;
    });


}
