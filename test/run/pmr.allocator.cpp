#include <felspar/memory/pmr.allocator.hpp>
#include <felspar/memory/slab.storage.hpp>
#include <felspar/test.hpp>

#include <felspar/exceptions/logic_error.hpp>


namespace {


    auto const suite = felspar::testsuite("pmr.allocator");


    auto const basic_alloc_dealloc = suite.test(
            "slab_storage allocate/deallocate round-trip", [](auto check) {
                felspar::memory::pmr_resource<felspar::memory::slab_storage<1024>>
                        resource;
                felspar::pmr::memory_resource *mr = &resource;

                auto *p1 = static_cast<std::byte *>(mr->allocate(100));
                check(p1 != nullptr);

                auto *p2 = static_cast<std::byte *>(mr->allocate(200));
                check(p2 != nullptr);
                check(p2 != p1);

                mr->deallocate(p2, 200, 8);
                mr->deallocate(p1, 100, 8);
            });


    auto const overaligned_throws =
            suite.test("overaligned allocation throws", [](auto check) {
                felspar::memory::pmr_resource<felspar::memory::slab_storage<1024>>
                        resource;
                felspar::pmr::memory_resource *mr = &resource;

                check([&]() {
                    [[maybe_unused]] auto *p =
                            mr->allocate(64, alignof(std::max_align_t) * 2);
                }).template throws_type<felspar::stdexcept::logic_error>();
            });


}
