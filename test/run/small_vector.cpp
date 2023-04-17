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


    struct count_destruction {
        static std::size_t count;
        ~count_destruction() { ++count; }
    };
    std::size_t count_destruction::count = {};
    auto const destruct = suite.test("destruction", [](auto check) {
        { [[maybe_unused]] count_destruction cd{}; }
        check(std::exchange(count_destruction::count, 0)) == 1u;

        {
            felspar::memory::small_vector<count_destruction> c;
            c.emplace_back();
            check(c.size()) == 1u;
        }
        check(std::exchange(count_destruction::count, 0)) == 1u;

        {
            felspar::memory::small_vector<count_destruction> c;
            c.emplace_back();
            c.emplace_back();
            check(c.size()) == 2u;
        }
        check(std::exchange(count_destruction::count, 0)) == 2u;
    });


    auto const erase = suite.test("erase", [](auto check) {
        felspar::memory::small_vector c1{1, 2, 3};
        c1.erase(c1.begin());
        check(c1.size()) == 2u;
        check(c1[0]) == 2;
        check(c1[1]) == 3;
        c1.erase(c1.begin() + 1);
        check(c1.size()) == 1u;
        check(c1[0]) == 2;
        c1.erase(c1.begin());
        check(c1.size()) == 0u;

        {
            felspar::memory::small_vector cd{
                    count_destruction{}, count_destruction{}};
            count_destruction::count = 0u;
            cd.erase(cd.begin());
            check(count_destruction::count) == 1u;
            cd.erase(cd.begin());
            check(count_destruction::count) == 2u;
        }
        check(count_destruction::count) == 2u;
    });


    struct no_assign {
        const std::string s;
    };
    auto const erase_m = suite.test("erase/no-assignment", [](auto check) {
        felspar::memory::small_vector c1{no_assign{"a"}, no_assign{"b"}};
        check(c1.size()) == 2u;
        c1.erase(c1.begin());
        check(c1.front().s) == "b";
    });


    auto const resize = suite.test("resize", [](auto check) {
        felspar::memory::small_vector c1{1, 2, 3};
        c1.resize(1);
        check(c1.size()) == 1u;
        check(c1.at(0)) == 1;

        c1.resize(0);
        check(c1.size()) == 0u;

        c1.push_back(4);
        check(c1.size()) == 1u;

        c1.resize(3, 5);
        check(c1.size()) == 3u;
        check(c1.at(0)) == 4;
        check(c1.at(1)) == 5;
        check(c1.at(2)) == 5;
    });


}
