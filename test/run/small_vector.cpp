#include <felspar/memory/small_vector.hpp>
#include <felspar/test.hpp>


namespace {


    auto const suite = felspar::testsuite("small_vector");


    auto const access = suite.test("accessors", [](auto check) {
        felspar::memory::small_vector<int> const int_c{0, 1, 2, 3, 4};
        check(int_c.front()) == 0;
        check(int_c[1]) == 1;
        check(int_c[2]) == 2;
        check(int_c[3]) == 3;
        check(int_c.back()) == 4;

        felspar::memory::small_vector<int> int_m{0, 1, 2, 3, 4};
        check(int_m.front()) == 0;
        int_m.front() = -1;
        check(int_m.front()) == -1;
        check(int_m[1]) == 1;
        check(int_m[2]) == 2;
        check(int_m[3]) == 3;
        check(int_m.back()) == 4;
        int_m.back() = -2;
        check(int_m.back()) == -2;
    });

    auto const meta = suite.test("meta", [](auto check) {
        felspar::memory::small_vector<int> c_int;
        check(c_int.capacity()) == 32u;
        check(c_int.size()) == 0u;
        felspar::memory::small_vector<std::array<std::byte, 58>, 128> c_56;
        check(c_56.capacity()) == 128u;
        check(c_56.size()) == 0u;
    });


    auto const cons = suite.test("iterate", [](auto check) {
        felspar::memory::small_vector const c_int{1, 2, 3};
        check(c_int.size()) == 3u;

        for (int expected = 0; auto const &v : c_int) {
            check(v) == ++expected;
        }
    });


    class emplace_only {
        std::string one, two;

      public:
        emplace_only(std::string o, std::string t)
        : one{std::move(o)}, two{std::move(t)} {}
        emplace_only(emplace_only const &) = delete;
        emplace_only(emplace_only &&) = delete;

        bool operator==(emplace_only const &) const = default;
    };

    auto const mod = suite.test("back insertion", [](auto check) {
        felspar::memory::small_vector<int> c_int;
        c_int.push_back(0);
        check(c_int.size()) == 1u;
        c_int.push_back(1);
        check(c_int.size()) == 2u;

        felspar::memory::small_vector<emplace_only> emp;
        emp.emplace_back("one", "two");
        check(emp.back()) == emplace_only{"one", "two"};
    });


}
