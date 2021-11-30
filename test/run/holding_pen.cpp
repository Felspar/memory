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


    auto const compare = suite.test("compare", [](auto check) {
        std::optional<int> ot_1;
        std::optional<int> ot_2;
        check(ot_1) == ot_2;

        felspar::memory::holding_pen<int> hp_1;
        felspar::memory::holding_pen<int> hp_2;
        check(hp_1) == hp_2;

        std::optional<int> ot_3;
        std::optional<int> ot_4{2};
        check(ot_3 == ot_4).is_falsey();

        felspar::memory::holding_pen<int> hp_3;
        felspar::memory::holding_pen<int> hp_4{2};
        check(hp_3 == hp_4).is_falsey();

        std::optional<int> ot_5{2};
        std::optional<int> ot_6{2};
        check(ot_5) == ot_6;

        felspar::memory::holding_pen<int> hp_5{2};
        felspar::memory::holding_pen<int> hp_6{2};
        check(hp_5) == hp_6;

        std::optional<int> ot_7{2};
        std::optional<int> ot_8{3};
        check(ot_7 == ot_8).is_falsey();

        felspar::memory::holding_pen<int> hp_7{2};
        felspar::memory::holding_pen<int> hp_8{3};
        check(hp_7 == hp_8).is_falsey();
    });


}
