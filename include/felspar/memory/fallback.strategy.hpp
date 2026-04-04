#pragma once

#include <felspar/memory/concepts.hpp>

#include <cstddef>
#include <source_location>
#include <type_traits>
#include <utility>


namespace felspar::memory {


    namespace detail {


        template<typename T, typename Tuple, std::size_t... Is>
        constexpr T construct_from_tuple(Tuple &&t, std::index_sequence<Is...>) {
            return T(std::get<Is>(std::forward<Tuple>(t))...);
        }


        template<typename T, typename Tuple>
        constexpr T construct_from_tuple(Tuple &&t) {
            constexpr auto size =
                    std::tuple_size_v<std::remove_reference_t<Tuple>>;
            return construct_from_tuple<T>(
                    std::forward<Tuple>(t), std::make_index_sequence<size>{});
        }


    }


    /**
     * ## Fallback allocator strategy
     *
     * A compositor that tries a primary strategy first and falls back to a
     * secondary strategy if the primary cannot satisfy the allocation request.
     * Deallocation is routed back to the correct strategy using the primary's
     * `owns()` method.
     *
     * The primary must satisfy `owning_allocator_strategy` (provides both
     * `try_allocate` and `owns`), while the fallback only needs to satisfy
     * `allocator_strategy`.
     *
     * This compositor owns both strategies by value. It is non-movable when
     * either strategy is non-movable.
     */
    template<owning_allocator_strategy Primary, allocator_strategy Fallback>
    class fallback_strategy {
      public:
        using primary_type = Primary;
        using fallback_type = Fallback;


        /// ### Construct with owned strategies
        fallback_strategy(primary_type p, fallback_type f) noexcept(
                std::is_nothrow_move_constructible_v<primary_type>
                and std::is_nothrow_move_constructible_v<fallback_type>)
        : primary(std::move(p)), fallback(std::move(f)) {}


        /// ### In-place construction for non-movable strategies
        template<typename... PArgs, typename... FArgs>
        fallback_strategy(std::piecewise_construct_t, std::tuple<PArgs...> pargs, std::tuple<FArgs...> fargs) noexcept(
                std::is_nothrow_constructible_v<primary_type, PArgs...>
                and std::is_nothrow_constructible_v<fallback_type, FArgs...>)
        : primary(detail::construct_from_tuple<primary_type>(std::move(pargs))),
          fallback(
                  detail::construct_from_tuple<fallback_type>(
                          std::move(fargs))) {}


        /// ### Non-copyable
        fallback_strategy(fallback_strategy const &) = delete;
        fallback_strategy &operator=(fallback_strategy const &) = delete;


        /// ### Movable only if both strategies are movable
        fallback_strategy(fallback_strategy &&) noexcept(
                std::is_nothrow_move_constructible_v<primary_type>
                and std::is_nothrow_move_constructible_v<fallback_type>)
            requires(std::is_move_constructible_v<primary_type>
                     and std::is_move_constructible_v<fallback_type>)
        = default;
        fallback_strategy &operator=(fallback_strategy &&) noexcept(
                std::is_nothrow_move_assignable_v<primary_type>
                and std::is_nothrow_move_assignable_v<fallback_type>)
            requires(std::is_move_assignable_v<primary_type>
                     and std::is_move_assignable_v<fallback_type>)
        = default;


        /// ### Throwing allocation
        [[nodiscard]] std::byte *allocate(
                std::size_t const bytes,
                std::source_location loc = std::source_location::current()) {
            if (auto *ptr = primary.try_allocate(bytes); ptr != nullptr) {
                return ptr;
            } else {
                return fallback.allocate(bytes, loc);
            }
        }


        /// ### Deallocate a previously allocated block
        void deallocate(void *ptr, std::size_t const bytes) noexcept(
                noexcept(std::declval<primary_type &>().deallocate(ptr, bytes))
                and noexcept(std::declval<fallback_type &>().deallocate(
                        ptr, bytes))) {
            if (primary.owns(ptr)) {
                primary.deallocate(ptr, bytes);
            } else {
                fallback.deallocate(ptr, bytes);
            }
        }


        /// ### Nullable allocation
        [[nodiscard]] std::byte *try_allocate(std::size_t const bytes) noexcept(
                noexcept(std::declval<primary_type &>().try_allocate(bytes))
                and noexcept(std::declval<fallback_type &>().allocate(bytes)))
            requires nullable_allocator_strategy<fallback_type>
        /**
         * The fallback must also support `try_allocate` for this to be exposed
         * here. If the fallback allocator satisfies
         * `nullable_allocator_strategy` then so does this combined allocator.
         */
        {
            if (auto *ptr = primary.try_allocate(bytes); ptr != nullptr) {
                return ptr;
            } else {
                return fallback.try_allocate(bytes);
            }
        }


        /// ### Ownership test
        [[nodiscard]] bool owns(void const *const ptr) noexcept(
                noexcept(std::declval<primary_type &>().owns(ptr))
                and noexcept(std::declval<fallback_type &>().owns(ptr)))
            requires owning_allocator_strategy<fallback_type>
        /**
         * The fallback must also support `owns` for this to be exposed here. If
         * the fallback allocator satisfies `owning_allocator_strategy` then so
         * does this combined allocator.
         */
        {
            return primary.owns(ptr) or fallback.owns(ptr);
        }


        /// ### Access the owned strategies
        primary_type &get_primary() noexcept { return primary; }
        primary_type const &get_primary() const noexcept { return primary; }

        fallback_type &get_fallback() noexcept { return fallback; }
        fallback_type const &get_fallback() const noexcept { return fallback; }


      private:
        primary_type primary;
        fallback_type fallback;
    };


}
