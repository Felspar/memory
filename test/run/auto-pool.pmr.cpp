#include <felspar/memory/auto-pool.pmr.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("auto-pool.pmr");


    auto const s = suite.test("small", [](auto check) {
        felspar::memory::auto_pool::pmr fp{
                512, felspar::pmr::new_delete_resource()};

        void *a1 = fp.allocate(100);
        void *a2 = fp.allocate(100);
        check(a1) != a2;
        fp.deallocate(a1, 100);
        fp.deallocate(a2, 100);

        void *a3 = fp.allocate(200);
        check(a3) == a2;
        fp.deallocate(a3, 200);
    });


    auto const b = suite.test("big", [](auto check) {
        felspar::memory::auto_pool::pmr fp{
                512, felspar::pmr::new_delete_resource()};

        void *a1 = fp.allocate(1000);
        void *a2 = fp.allocate(1000);
        check(a1) != a2;
        fp.deallocate(a1, 1000);
        fp.deallocate(a2, 1000);

        void *a3 = fp.allocate(2000);
        check(a3) != a2;
        fp.deallocate(a3, 2000);
    });


}
