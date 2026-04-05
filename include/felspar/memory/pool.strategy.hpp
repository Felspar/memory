#pragma once


#include <felspar/memory/concepts.hpp>
#include <felspar/memory/exceptions.hpp>
#include <felspar/memory/new_delete.strategy.hpp>

#include <array>
#include <type_traits>
#include <vector>


namespace felspar::memory {


    /**
     * ## Pool allocator strategy — `pool_strategy` and `pool`
     *
     * A pooling memory manager that serves allocations up to the largest size
     * bracket.
     *
     * Two types are provided here. `pool_strategy<Sizes, Fallback>` is the full
     * template where both the size array and the fallback allocator type are
     * explicit template parameters — use this when you need a custom fallback.
     * `pool<Sizes...>` is a convenience alias that fixes the fallback to
     * `new_delete_strategy` and lets you list sizes directly as non-type
     * parameters — use this for the common case.
     *
     * ```cpp
     * // pool<Sizes...> — default new/delete fallback, sizes as pack:
     * felspar::memory::pool<8, 16, 64> ps;
     *
     * // pool_strategy — custom fallback, sizes as std::array NTTP:
     * felspar::memory::pool_strategy<
     *         std::array<std::size_t, 3>{8, 16, 64}, my_fallback_type>
     *         ps{my_fallback};
     * ```
     *
     * Both manage a set of size-class free lists backed by the fallback
     * allocator. **Sizes must be listed in ascending order** — this is
     * enforced at compile time via a `static_assert`.
     *
     * Each tier holds a `std::vector` of available blocks. On allocation the
     * smallest fitting tier is tried first; if it has a free block that block
     * is returned immediately. If not, a new block of the tier size is obtained
     * from the fallback. Deallocated blocks that fit a tier are returned to
     * that tier's free list. Oversized allocations bypass the pool entirely.
     *
     * The pool owns all free-listed blocks and returns them to the fallback on
     * destruction.
     *
     * ### Concept satisfaction
     *
     * Satisfies `nullable_allocator_strategy` — `try_allocate` returns
     * `nullptr` rather than throwing when no free block is available (without
     * growing the pool). Does not satisfy `owning_allocator_strategy` because
     * blocks are sourced from the fallback at arbitrary addresses.
     *
     * ### Note on `deallocate` exception safety
     *
     * `deallocate` may throw if the underlying `std::vector` needs to grow its
     * capacity while returning a block to the free list. Pre-reserving capacity
     * via `reserve(tier, n)` eliminates this risk for known workloads.
     */
    template<auto Sizes, allocator_strategy Fallback = new_delete_strategy>
        requires std::same_as<typename decltype(Sizes)::value_type, std::size_t>
    class pool_strategy final {
        static constexpr std::size_t N = Sizes.size();
        std::array<std::vector<std::byte *>, N> m_pools;
        Fallback m_fallback;

        static_assert(
                []() {
                    for (std::size_t i{1}; i < N; ++i) {
                        if (Sizes[i] <= Sizes[i - 1]) { return false; }
                    }
                    return true;
                }(),
                "pool_strategy: Sizes must be strictly ascending");


      public:
        using fallback_type = Fallback;
        static std::array<std::size_t, N> constexpr sizes = Sizes;


        /// ### Construct with an optional fallback strategy
        pool_strategy()
            requires std::is_default_constructible_v<Fallback>
        = default;

        explicit pool_strategy(Fallback fallback)
        : m_fallback{std::move(fallback)} {}

        /// #### Construct via a factory lambda
        template<typename Factory>
            requires std::same_as<std::invoke_result_t<Factory>, Fallback>
        explicit pool_strategy(Factory &&factory)
        /**
         * Pass a callable that returns a `Fallback` instance. The return value
         * is constructed directly into `m_fallback` via guaranteed copy
         * elision, so this works even when `Fallback` is neither copyable nor
         * movable.
         *
         * ```cpp
         * pool_strategy ps{[&]{ return MyFallback{params}; }};
         * ```
         */
        : m_fallback{std::forward<Factory>(factory)()} {}


        /// ### Non-copyable
        pool_strategy(pool_strategy const &) = delete;
        pool_strategy &operator=(pool_strategy const &) = delete;


        /// ### Movable if the fallback is movable
        pool_strategy(pool_strategy &&) noexcept(
                std::is_nothrow_move_constructible_v<Fallback>)
            requires std::is_move_constructible_v<Fallback>
        = default;
        pool_strategy &operator=(pool_strategy &&) noexcept(
                std::is_nothrow_move_assignable_v<Fallback>)
            requires std::is_move_assignable_v<Fallback>
        = default;


        /// ### Return all free-listed blocks to the fallback
        ~pool_strategy() {
            for (std::size_t i{}; i < N; ++i) {
                for (auto *ptr : m_pools[i]) {
                    m_fallback.deallocate(ptr, Sizes[i]);
                }
            }
        }


        /// ### Pre-reserve free-list capacity for a tier
        /**
         * Reserves space in the free-list vector for tier `i`, preventing
         * `deallocate` from throwing due to a vector reallocation.
         */
        void reserve(std::size_t const tier, std::size_t const capacity) {
            m_pools[tier].reserve(capacity);
        }


        /// ### Allocate from the free list, or grow via the fallback
        /**
         * Returns `nullptr` only for oversized requests (larger than every
         * tier). For a matching tier, returns a free block if one is available,
         * otherwise allocates a new block of the tier size from the fallback.
         * May throw if the fallback throws.
         */
        [[nodiscard]] std::byte *try_allocate(std::size_t const bytes) {
            return try_allocate_at_least(bytes).ptr;
        }
        [[nodiscard]] allocation_result
                try_allocate_at_least(std::size_t const bytes) {
            for (std::size_t i{}; i < N; ++i) {
                if (Sizes[i] >= bytes) {
                    if (not m_pools[i].empty()) {
                        auto *const ptr = m_pools[i].back();
                        m_pools[i].pop_back();
                        return {ptr, Sizes[i]};
                    }
                    return {m_fallback.allocate(Sizes[i]), Sizes[i]};
                }
            }
            return {nullptr, {}};
        }


        /// ### Allocate, throwing for oversized requests
        /**
         * Thin wrapper around `allocate_at_least`. Throws `std::bad_alloc`
         * when the request is larger than every tier. Compose with
         * `fallback_strategy` to handle those separately.
         */
        [[nodiscard]] std::byte *allocate(
                std::size_t const bytes,
                std::source_location const loc =
                        std::source_location::current()) {
            return allocate_at_least(bytes, loc).ptr;
        }
        [[nodiscard]] allocation_result allocate_at_least(
                std::size_t const bytes,
                std::source_location const loc =
                        std::source_location::current()) {
            if (auto result = try_allocate_at_least(bytes); result.ptr) {
                return result;
            }
            detail::throw_bad_alloc("pool_strategy oversized allocation", loc);
        }


        /// ### Return a block to the appropriate free list
        void deallocate(void *const ptr, std::size_t const bytes)
        /**
         * The size must fit within one of the pool tiers. Passing an oversized
         * `bytes` value is undefined behaviour — it will never occur for
         * pointers obtained from `allocate`.
         */
        {
            for (std::size_t i{}; i < N; ++i) {
                if (Sizes[i] >= bytes) {
                    m_pools[i].push_back(static_cast<std::byte *>(ptr));
                    return;
                }
            }
        }
    };


    /// ## Convenience alias with the default new/delete fallback
    /**
     * ```cpp
     * felspar::memory::pool<8, 16, 64> ps;
     * ```
     */
    template<std::size_t... Sizes>
    using pool =
            pool_strategy<std::array<std::size_t, sizeof...(Sizes)>{Sizes...}>;


}
