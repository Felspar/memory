#include <felspar/memory/bitmap.strategy.hpp>
#include <felspar/memory/concepts.hpp>
#include <felspar/test.hpp>

#include <felspar/exceptions/bad_alloc.hpp>

#include <array>
#include <cstdint>


static_assert(felspar::memory::allocator_strategy<
              felspar::memory::bitmap_strategy<std::uint8_t>>);
static_assert(felspar::memory::nullable_allocator_strategy<
              felspar::memory::bitmap_strategy<std::uint8_t>>);
static_assert(felspar::memory::owning_allocator_strategy<
              felspar::memory::bitmap_strategy<std::uint8_t>>);
static_assert(felspar::memory::overallocating_allocator_strategy<
              felspar::memory::bitmap_strategy<std::uint8_t>>);


namespace {


    auto const suite = felspar::testsuite("bitmap.strategy");


    auto const alloc_dealloc =
            suite.test("allocate/deallocate", [](auto check) {
                std::array<std::byte, 256> storage;
                felspar::memory::bitmap_strategy<std::uint8_t> strategy(
                        storage.data(), 32);

                auto *p1 = strategy.allocate(32);
                check(p1) == storage.data();
                check(strategy.owns(p1)) == true;

                auto *p2 = strategy.allocate(32);
                check(p2) == storage.data() + 32;
                check(strategy.owns(p2)) == true;

                strategy.deallocate(p1, 32);
                strategy.deallocate(p2, 32);
                check(strategy.owns(p1)) == true;
                check(strategy.owns(p2)) == true;
            });


    auto const owns = suite.test("owns", [](auto check) {
        std::array<std::byte, 256> storage;
        felspar::memory::bitmap_strategy<std::uint8_t> strategy(
                storage.data(), 32);

        check(strategy.owns(storage.data())) == true;
        check(strategy.owns(storage.data() + 32)) == true;
        check(strategy.owns(storage.data() + 255)) == true;
        check(strategy.owns(storage.data() + 256)) == false;
        check(strategy.owns(storage.data() - 1)) == false;
        check(strategy.owns(nullptr)) == false;
    });


    auto const try_allocate_full = suite.test(
            "try_allocate returns nullptr when full", [](auto check) {
                std::array<std::byte, 256> storage;
                felspar::memory::bitmap_strategy<std::uint8_t> strategy(
                        storage.data(), 32);

                for (std::size_t i = 0; i < 8; ++i) {
                    auto *p = strategy.try_allocate(32);
                    check(p != nullptr);
                }

                auto *p = strategy.try_allocate(32);
                check(p) == nullptr;
            });


    auto const allocate_throws = suite.test("allocate throws when full", [](auto check) {
        std::array<std::byte, 256> storage;
        felspar::memory::bitmap_strategy<std::uint8_t> strategy(
                storage.data(), 32);

        for (std::size_t i = 0; i < 8; ++i) {
            auto *p = strategy.allocate(32);
            check(p != nullptr);
        }

        check([&]() { [[maybe_unused]] auto _ = strategy.allocate(32); })
                .throws(felspar::stdexcept::bad_alloc{
                        "bitmap_strategy pool exhausted or oversized allocation"});
    });


    auto const oversized = suite.test("rejects oversized requests", [](auto check) {
        std::array<std::byte, 256> storage;
        felspar::memory::bitmap_strategy<std::uint8_t> strategy(
                storage.data(), 32);

        check(strategy.try_allocate(64)) == nullptr;

        check([&]() { [[maybe_unused]] auto _ = strategy.allocate(64); })
                .throws(felspar::stdexcept::bad_alloc{
                        "bitmap_strategy pool exhausted or oversized allocation"});
    });


    auto const capacity = suite.test("capacity", [](auto check) {
        std::array<std::byte, 256> storage;
        felspar::memory::bitmap_strategy<std::uint8_t> strategy(
                storage.data(), 32);

        check(strategy.capacity()) == 8u;
    });


    auto const alloc_at_least =
            suite.test("allocate_at_least reports block size", [](auto check) {
                std::array<std::byte, 64 * 8> storage;
                felspar::memory::bitmap_strategy<std::uint8_t> strategy{
                        storage.data(), 64};

                auto result = strategy.allocate_at_least(10);
                check(result.ptr != nullptr);
                check(result.bytes) == 64u;

                strategy.deallocate(result.ptr, result.bytes);
            });


}
