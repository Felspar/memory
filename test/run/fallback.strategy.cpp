#include <felspar/memory/bitmap.strategy.hpp>
#include <felspar/memory/concepts.hpp>
#include <felspar/memory/fallback.strategy.hpp>
#include <felspar/memory/stack.storage.hpp>
#include <felspar/test.hpp>

#include <array>
#include <cstdint>
#include <memory>
#include <tuple>


static_assert(
        felspar::memory::allocator_strategy<felspar::memory::fallback_strategy<
                felspar::memory::bitmap_strategy<std::uint8_t>,
                felspar::memory::stack_storage<>>>);
static_assert(not felspar::memory::nullable_allocator_strategy<
              felspar::memory::fallback_strategy<
                      felspar::memory::bitmap_strategy<std::uint8_t>,
                      felspar::memory::stack_storage<>>>);
static_assert(felspar::memory::nullable_allocator_strategy<
              felspar::memory::fallback_strategy<
                      felspar::memory::bitmap_strategy<std::uint8_t>,
                      felspar::memory::bitmap_strategy<std::uint8_t>>>);
static_assert(not felspar::memory::owning_allocator_strategy<
              felspar::memory::fallback_strategy<
                      felspar::memory::bitmap_strategy<std::uint8_t>,
                      felspar::memory::stack_storage<>>>);
static_assert(felspar::memory::owning_allocator_strategy<
              felspar::memory::fallback_strategy<
                      felspar::memory::bitmap_strategy<std::uint8_t>,
                      felspar::memory::bitmap_strategy<std::uint8_t>>>);
static_assert(felspar::memory::overallocating_allocator_strategy<
              felspar::memory::fallback_strategy<
                      felspar::memory::bitmap_strategy<std::uint8_t>,
                      felspar::memory::bitmap_strategy<std::uint8_t>>>);
static_assert(not felspar::memory::overallocating_allocator_strategy<
              felspar::memory::fallback_strategy<
                      felspar::memory::bitmap_strategy<std::uint8_t>,
                      felspar::memory::stack_storage<>>>);


namespace {


    auto const suite = felspar::testsuite("fallback.strategy");


    auto const alloc_dealloc =
            suite.test("allocate/deallocate routing", [](auto check) {
                std::array<std::byte, 256> primary_storage;

                felspar::memory::fallback_strategy<
                        felspar::memory::bitmap_strategy<std::uint8_t>,
                        felspar::memory::stack_storage<1024>>
                        strategy(
                                std::piecewise_construct,
                                std::make_tuple(primary_storage.data(), 32),
                                std::tuple<>{});

                // First allocation should come from primary
                auto *p1 = strategy.allocate(32);
                check(strategy.get_primary().owns(p1)) == true;

                // Second allocation should also come from primary
                auto *p2 = strategy.allocate(32);
                check(strategy.get_primary().owns(p2)) == true;

                // Fill up the primary (8 blocks of 32 bytes)
                for (std::size_t i = 2; i < 8; ++i) {
                    auto *p = strategy.allocate(32);
                    check(strategy.get_primary().owns(p)) == true;
                }

                // Next allocation should fall through to fallback
                auto *p_fallback = strategy.allocate(32);
                check(strategy.get_primary().owns(p_fallback)) == false;

                // Deallocate primary-owned pointer should route to primary
                strategy.deallocate(p1, 32);

                // Next allocation should come from primary again (slot freed)
                auto *p3 = strategy.allocate(32);
                check(strategy.get_primary().owns(p3)) == true;
            });


    auto const fallback_route =
            suite.test("fallback allocation when primary full", [](auto check) {
                std::array<std::byte, 64> primary_storage;
                auto fallback_storage = std::make_unique<std::byte[]>(1024);

                felspar::memory::fallback_strategy<
                        felspar::memory::bitmap_strategy<std::uint8_t>,
                        felspar::memory::bitmap_strategy<std::uint8_t>>
                        strategy(
                                std::piecewise_construct,
                                std::make_tuple(primary_storage.data(), 8),
                                std::make_tuple(fallback_storage.get(), 8));

                // Fill primary completely
                std::array<void *, 8> primary_ptrs{};
                for (std::size_t i = 0; i < 8; ++i) {
                    primary_ptrs[i] = strategy.allocate(8);
                    check(strategy.get_primary().owns(primary_ptrs[i])) == true;
                }

                // This allocation must come from fallback
                auto *p_fallback = strategy.allocate(8);
                check(strategy.get_primary().owns(p_fallback)) == false;

                // Deallocate the fallback allocation
                strategy.deallocate(p_fallback, 8);

                // Allocate and deallocate from fallback again
                auto *p_fallback2 = strategy.allocate(8);
                check(strategy.get_primary().owns(p_fallback2)) == false;
                strategy.deallocate(p_fallback2, 8);
            });


    auto const alloc_at_least = suite.test(
            "allocate_at_least routes and reports block size", [](auto check) {
                std::array<std::byte, 64> primary_storage;
                auto fallback_storage = std::make_unique<std::byte[]>(512);

                felspar::memory::fallback_strategy<
                        felspar::memory::bitmap_strategy<std::uint8_t>,
                        felspar::memory::bitmap_strategy<std::uint8_t>>
                        strategy(
                                std::piecewise_construct,
                                std::make_tuple(primary_storage.data(), 8),
                                std::make_tuple(fallback_storage.get(), 8));

                // Fill primary, then allocate_at_least should fall back
                for (std::size_t i = 0; i < 8; ++i) {
                    auto result = strategy.allocate_at_least(4);
                    check(result.ptr != nullptr);
                    check(result.bytes) == 8u;
                }

                // Next must come from fallback — block size still reported
                auto result = strategy.allocate_at_least(4);
                check(result.ptr != nullptr);
                check(result.bytes) == 8u;
                check(strategy.get_primary().owns(result.ptr)) == false;

                strategy.deallocate(result.ptr, result.bytes);
            });


    auto const oversized = suite.test(
            "oversized requests fall through to fallback", [](auto check) {
                std::array<std::byte, 256> primary_storage;

                felspar::memory::fallback_strategy<
                        felspar::memory::bitmap_strategy<std::uint8_t>,
                        felspar::memory::stack_storage<1024>>
                        strategy(
                                std::piecewise_construct,
                                std::make_tuple(primary_storage.data(), 32),
                                std::tuple<>{});

                // Request larger than block size should go directly to fallback
                auto *p = strategy.allocate(64);
                check(strategy.get_primary().owns(p)) == false;

                // And deallocation should route correctly
                strategy.deallocate(p, 64);
            });


}
