#pragma once

#include <felspar/memory/concepts.hpp>

#include <cstddef>
#include <tuple>
#include <type_traits>


namespace felspar::memory {


    /**
     * ## Size-split allocator strategy
     *
     * A compositor that routes allocation requests based on size. Allocations
     * at or below the threshold go to the "small" strategy; allocations above
     * the threshold go to the "large" strategy. Deallocation is routed by the
     * same size threshold.
     *
     * Both strategies must satisfy `allocator_strategy`. This compositor is
     * safe to use with PMR and stdlib allocator wrappers because those pass
     * the original allocation size to deallocate. It is NOT safe for raw
     * `operator delete` usage since the size is not preserved.
     *
     * This compositor owns both strategies by value. It is non-movable when
     * either strategy is non-movable.
     */
    template<allocator_strategy Small, allocator_strategy Large>
    class size_split_strategy {
      public:
        using small_type = Small;
        using large_type = Large;


        /// ### Construct with owned strategies and threshold
        size_split_strategy(small_type s, large_type l, std::size_t t) noexcept(
                std::is_nothrow_move_constructible_v<small_type>
                and std::is_nothrow_move_constructible_v<large_type>)
        : small(std::move(s)), large(std::move(l)), threshold(t) {}


        /// ### In-place construction for non-movable strategies
        template<typename... SArgs, typename... LArgs>
        size_split_strategy(std::piecewise_construct_t, std::tuple<SArgs...> sargs, std::tuple<LArgs...> largs, std::size_t t) noexcept(
                std::is_nothrow_constructible_v<small_type, SArgs...>
                and std::is_nothrow_constructible_v<large_type, LArgs...>)
        : small(std::make_from_tuple<small_type>(std::move(sargs))),
          large(std::make_from_tuple<large_type>(std::move(largs))),
          threshold(t) {}


        /// ### Non-copyable
        size_split_strategy(size_split_strategy const &) = delete;
        size_split_strategy &operator=(size_split_strategy const &) = delete;


        /// ### Movable only if both strategies are movable
        size_split_strategy(size_split_strategy &&) noexcept(
                std::is_nothrow_move_constructible_v<small_type>
                and std::is_nothrow_move_constructible_v<large_type>)
            requires(std::is_move_constructible_v<small_type>
                     and std::is_move_constructible_v<large_type>)
        = default;
        size_split_strategy &operator=(size_split_strategy &&) noexcept(
                std::is_nothrow_move_assignable_v<small_type>
                and std::is_nothrow_move_assignable_v<large_type>)
            requires(std::is_move_assignable_v<small_type>
                     and std::is_move_assignable_v<large_type>)
        = default;


        /// ### Allocate, routing by size threshold
        [[nodiscard]] std::byte *allocate(std::size_t const bytes) {
            if (bytes <= threshold) {
                return small.allocate(bytes);
            } else {
                return large.allocate(bytes);
            }
        }


        /// ### Throwing over-allocation, routing by size threshold
        [[nodiscard]] allocation_result
                allocate_at_least(std::size_t const bytes)
            requires overallocating_allocator_strategy<small_type>
                and overallocating_allocator_strategy<large_type>
        {
            if (bytes <= threshold) {
                return small.allocate_at_least(bytes);
            } else {
                return large.allocate_at_least(bytes);
            }
        }


        /// ### Nullable over-allocation, routing by size threshold
        [[nodiscard]] allocation_result
                try_allocate_at_least(std::size_t const bytes) noexcept(
                        noexcept(std::declval<small_type &>()
                                         .try_allocate_at_least(bytes))
                        and noexcept(std::declval<large_type &>()
                                             .try_allocate_at_least(bytes)))
            requires overallocating_allocator_strategy<small_type>
                and overallocating_allocator_strategy<large_type>
        {
            if (bytes <= threshold) {
                return small.try_allocate_at_least(bytes);
            } else {
                return large.try_allocate_at_least(bytes);
            }
        }


        /// ### Deallocate, routing by size threshold
        void deallocate(void *ptr, std::size_t const bytes) noexcept(
                noexcept(std::declval<small_type &>().deallocate(ptr, bytes))
                and noexcept(
                        std::declval<large_type &>().deallocate(ptr, bytes))) {
            if (bytes <= threshold) {
                small.deallocate(ptr, bytes);
            } else {
                large.deallocate(ptr, bytes);
            }
        }


        /// ### Access the owned strategies
        small_type &get_small() noexcept { return small; }
        small_type const &get_small() const noexcept { return small; }

        large_type &get_large() noexcept { return large; }
        large_type const &get_large() const noexcept { return large; }

        /// ### Access the threshold
        [[nodiscard]] std::size_t get_threshold() const noexcept {
            return threshold;
        }


      private:
        small_type small;
        large_type large;
        std::size_t threshold;
    };


}
