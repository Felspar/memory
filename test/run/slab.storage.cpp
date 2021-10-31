#include <felspar/memory/slab.storage.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("slab");


    auto const ga = suite.test("good alloc", [](auto check) {
        constexpr std::size_t size = 1u << 10;
        felspar::memory::slab_storage<size, 8u> slab;
        check(slab.free()) == size;

        auto a1 = slab.allocate(8u);
        check(a1) == reinterpret_cast<std::byte const *>(&slab);
        check(slab.free()) == size - 8u;

        auto a2 = slab.allocate(1u);
        check(a2) == reinterpret_cast<std::byte const *>(&slab) + 8u;
        check(slab.free()) == size - 16u;

        slab.deallocate(a1);
        slab.deallocate(a2);
        check(slab.free()) == (1u << 10) - 16u;
    });


}
