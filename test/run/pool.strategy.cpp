#include <felspar/memory/concepts.hpp>
#include <felspar/memory/pool.strategy.hpp>
#include <felspar/exceptions/bad_alloc.hpp>
#include <felspar/test.hpp>

#include <cstddef>
#include <cstring>


static_assert(felspar::memory::nullable_allocator_strategy<
              felspar::memory::pool<8, 16, 32>>);
static_assert(not felspar::memory::owning_allocator_strategy<
              felspar::memory::pool<8, 16, 32>>);
static_assert(felspar::memory::overallocating_allocator_strategy<
              felspar::memory::pool<8, 16, 32>>);

static_assert(not std::is_copy_constructible_v<felspar::memory::pool<8, 16>>);
static_assert(std::is_move_constructible_v<felspar::memory::pool<8, 16>>);
static_assert(std::is_default_constructible_v<felspar::memory::pool<8, 16>>);

static_assert(felspar::memory::nullable_allocator_strategy<
              felspar::memory::pool_strategy<
                      std::array{8uz, 16uz},
                      felspar::memory::new_delete_strategy>>);


namespace {


    auto const suite = felspar::testsuite("pool.strategy");


    auto const grow_and_recycle = suite.test(
            "try_allocate uses fallback on empty pool, recycles on dealloc",
            [](auto check) {
                felspar::memory::pool<16, 64> ps;

                // First allocation — pool is empty, goes to fallback
                auto *p1 = ps.try_allocate(16);
                check(p1 != nullptr);

                // Return to free list
                ps.deallocate(p1, 16);

                // Now try_allocate should find the cached block
                auto *p2 = ps.try_allocate(16);
                check(p2) == p1;

                ps.deallocate(p2, 16);
            });


    auto const tier_routing = suite.test(
            "allocation routes to smallest fitting tier", [](auto check) {
                felspar::memory::pool<8, 32, 128> ps;

                // 6-byte request uses the 8-byte tier
                auto *small = ps.try_allocate(6);
                check(small != nullptr);
                ps.deallocate(small, 6);

                // After dealloc, try_allocate(6) finds the recycled 8-byte block
                auto *recycled = ps.try_allocate(6);
                check(recycled) == small;

                // 9-byte request skips the 8-byte tier and uses the 32-byte tier
                auto *medium = ps.try_allocate(9);
                check(medium != nullptr);
                check(medium != small);

                ps.deallocate(recycled, 6);
                ps.deallocate(medium, 9);
            });


    auto const oversized =
            suite.test("oversized requests fail", [](auto check) {
                felspar::memory::pool<8, 16> ps;

                // try_allocate returns nullptr for oversized
                check(ps.try_allocate(256)) == nullptr;

                // allocate throws for oversized
                check([&]() { [[maybe_unused]] auto _ = ps.allocate(256); })
                        .throws(felspar::stdexcept::bad_alloc{
                                "pool_strategy oversized allocation"});
            });


    auto const alloc_at_least = suite.test(
            "allocate_at_least reports tier block size", [](auto check) {
                felspar::memory::pool<16, 64> ps;

                // Request smaller than first tier — reports actual tier size
                auto small_result = ps.allocate_at_least(10);
                check(small_result.ptr != nullptr);
                check(small_result.bytes) == 16u;

                // Request exactly at second tier boundary
                auto large_result = ps.allocate_at_least(64);
                check(large_result.ptr != nullptr);
                check(large_result.bytes) == 64u;

                // try_allocate_at_least returns {nullptr, 0} for oversized
                auto oversized = ps.try_allocate_at_least(256);
                check(oversized.ptr) == nullptr;
                check(oversized.bytes) == 0u;

                ps.deallocate(small_result.ptr, small_result.bytes);
                ps.deallocate(large_result.ptr, large_result.bytes);
            });


    auto const multi_tier =
            suite.test("multiple tiers coexist independently", [](auto check) {
                felspar::memory::pool<8, 64> ps;

                auto *s = ps.allocate(8);
                auto *l = ps.allocate(64);
                check(s != nullptr);
                check(l != nullptr);
                check(s != l);

                ps.deallocate(s, 8);
                ps.deallocate(l, 64);

                // Each block returns to its own tier
                auto *rs = ps.try_allocate(8);
                auto *rl = ps.try_allocate(64);
                check(rs) == s;
                check(rl) == l;

                ps.deallocate(rs, 8);
                ps.deallocate(rl, 64);
            });


}
