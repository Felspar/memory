#include <felspar/memory/slab.storage.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("slab");


    auto const ga = suite.test("good alloc", [](auto check) {
        felspar::memory::slab_storage slab;
        check(slab.free()) == 16u << 10;

        auto a1 = slab.allocate(8u);
        check(a1) == reinterpret_cast<std::byte const *>(&slab);
        check(slab.free()) == (16u << 10) - 8u;

        auto a2 = slab.allocate(1u);
        check(a2) == reinterpret_cast<std::byte const *>(&slab) + 8u;
        check(slab.free()) == (16u << 10) - 16u;

        slab.deallocate(a1);
        slab.deallocate(a2);
        check(slab.free()) == (16u << 10) - 16u;
    });


}
