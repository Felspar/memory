#include <felspar/memory/holding_pen.hpp>
#include <felspar/test.hpp>

#include <optional>


namespace {


    auto const suite = felspar::testsuite("holding_pen");


    auto const access = suite.test("value", [](auto check) {
        std::optional<int> ot_1;
        check(ot_1).is_falsey();
        ot_1 = 5;
        check(ot_1).is_truthy();

        felspar::memory::holding_pen<int> hp_1;
        check(hp_1).is_falsey();
        hp_1 = 5;
        check(hp_1).is_truthy();

        std::optional<int> ot_2{2};
        check(ot_2.value()) == 2;
        ot_2.value() = 5;
        check(ot_2) == 5;

        felspar::memory::holding_pen<int> hp_2{2};
        check(hp_2.value()) == 2;
        hp_2 = 5;
        check(hp_2.value()) == 5;
    });


    auto const get_value_or_default = suite.test("value_or", [](auto check) {
        std::optional<int> ot_1;
        check(ot_1.value_or(2)) == 2;

        felspar::memory::holding_pen<int> hp_1;
        check(hp_1.value_or(2)) == 2;

        std::optional<int> ot_2;
        check(ot_2.value_or(2.99)) == 2;

        felspar::memory::holding_pen<int> hp_2;
        check(hp_2.value_or(2.99)) == 2;

        std::optional<int> ot_3{4};
        check(ot_3.value_or(2.99)) == 4;

        felspar::memory::holding_pen<int> hp_3{4};
        check(hp_3.value_or(2.99)) == 4;
    });


}
