#include <felspar/memory/new_delete.strategy.hpp>
#include <felspar/memory/concepts.hpp>
#include <felspar/test.hpp>

#include <cstddef>
#include <cstring>


static_assert(felspar::memory::allocator_strategy<
              felspar::memory::new_delete_strategy>);

static_assert(
        std::is_default_constructible_v<felspar::memory::new_delete_strategy>);
static_assert(
        std::is_copy_constructible_v<felspar::memory::new_delete_strategy>);
static_assert(
        std::is_move_constructible_v<felspar::memory::new_delete_strategy>);
static_assert(std::is_copy_assignable_v<felspar::memory::new_delete_strategy>);
static_assert(std::is_move_assignable_v<felspar::memory::new_delete_strategy>);


namespace {


    auto const suite = felspar::testsuite("new_delete.strategy");


    auto const alloc_dealloc_write =
            suite.test("allocate, write, deallocate", [](auto check) {
                felspar::memory::new_delete_strategy strategy;

                std::size_t const size = 128;
                auto *ptr = strategy.allocate(size);

                check(ptr != nullptr);

                std::memset(ptr, 0x42, size);
                check(static_cast<std::byte *>(ptr)[0]) == std::byte{0x42};
                check(static_cast<std::byte *>(ptr)[size - 1])
                        == std::byte{0x42};

                strategy.deallocate(ptr, size);
            });


    auto const stateless = suite.test("is stateless", [](auto check) {
        felspar::memory::new_delete_strategy s1;
        felspar::memory::new_delete_strategy s2;

        check(sizeof(s1)) == 1u;

        std::byte *ptr1 = s1.allocate(64);
        std::byte *ptr2 = s2.allocate(64);

        check(ptr1 != nullptr);
        check(ptr2 != nullptr);

        s1.deallocate(ptr1, 64);
        s2.deallocate(ptr2, 64);
    });


}
