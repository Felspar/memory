#include <felspar/memory/pmr.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("pmr");


    auto const eq = suite.test("is_equal", [](auto check) {
        check(felspar::pmr::new_delete_resource()->is_equal(
                *felspar::pmr::new_delete_resource()))
                == true;
    });


}
