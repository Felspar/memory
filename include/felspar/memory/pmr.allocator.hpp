#pragma once

#include <felspar/memory/concepts.hpp>
#include <felspar/memory/exceptions.hpp>
#include <felspar/memory/pmr.hpp>

#include <cstddef>
#include <source_location>
#include <utility>


namespace felspar::memory {


    /**
     * ## PMR memory resource adapter
     *
     * Wraps any allocator strategy as a `pmr::memory_resource`, enabling use
     * anywhere a `pmr::memory_resource*` is accepted. Owns the strategy by
     * value.
     *
     * The PMR interface requires allocation with alignment. This wrapper
     * rejects over-aligned allocations (greater than
     * `alignof(std::max_align_t)`) with the same error as `fixed_pool`.
     */
    template<allocator_strategy Strategy>
    class pmr_resource final : public pmr::memory_resource {
        Strategy strategy;


      public:
        using strategy_type = Strategy;


        /// ### Construct from a strategy
        explicit pmr_resource(strategy_type s) noexcept(
                std::is_nothrow_move_constructible_v<strategy_type>)
        : strategy{std::move(s)} {}


        /// ### Variadic forwarding constructor for in-place construction
        template<typename... Args>
        explicit pmr_resource(Args &&...args) noexcept(
                std::is_nothrow_constructible_v<strategy_type, Args...>)
        : strategy{std::forward<Args>(args)...} {}


        /// ### Non-copyable
        pmr_resource(pmr_resource const &) = delete;
        pmr_resource &operator=(pmr_resource const &) = delete;


        /// ### Movable if the strategy is movable
        pmr_resource(pmr_resource &&) noexcept(
                std::is_nothrow_move_constructible_v<strategy_type>)
            requires(std::is_move_constructible_v<strategy_type>)
        = default;
        pmr_resource &operator=(pmr_resource &&) noexcept(
                std::is_nothrow_move_assignable_v<strategy_type>)
            requires(std::is_move_assignable_v<strategy_type>)
        = default;


      private:
        void *do_allocate(
                std::size_t const bytes, std::size_t const alignment) override {
            if (alignment > alignof(std::max_align_t)) {
                detail::throw_overaligned_memory(
                        alignment, std::source_location::current());
            }
            return strategy.allocate(bytes);
        }


        void do_deallocate(
                void *const p, std::size_t const bytes, std::size_t) override {
            strategy.deallocate(p, bytes);
        }


        bool do_is_equal(
                pmr::memory_resource const &other) const noexcept override {
            return this == &other;
        }
    };


}
