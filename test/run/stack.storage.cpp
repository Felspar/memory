#include <felspar/memory/stack.storage.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("stack.storage");

    auto const ga = suite.test("good alloc", [](auto check) {
        felspar::memory::stack_storage stack;

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
            stack.allocate(65);
        }).throws(std::runtime_error{"Out of free memory"});

        auto a1 = stack.allocate(1u);
        check(a1) == reinterpret_cast<std::byte const *>(&stack);
        check(stack.free()) == stack.storage_bytes - 16u;

        auto a2 = stack.allocate(1u);
        check(a2) == reinterpret_cast<std::byte const *>(&stack) + 16u;
        check(stack.free()) == stack.storage_bytes - 32u;

        check([&]() {
            stack.allocate(8u);
        }).throws(std::runtime_error{"Out of allocation bookkeeping slots"});
    });


}
