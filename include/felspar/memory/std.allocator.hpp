#pragma once

#include <felspar/memory/concepts.hpp>

#include <cstddef>
#include <limits>
#include <memory>


namespace felspar::memory {


    /**
     * ## stdlib allocator adapter
     *
     * Wraps any allocator strategy as a `std::allocator<T>`, enabling use with
     * `std::vector`, `std::map`, and other standard containers. Holds the
     * strategy by non-owning pointer so copies compare equal, as required by
     * the stdlib allocator model.
     */
    template<typename T, allocator_strategy Strategy>
    class allocator {

        template<typename U, allocator_strategy S>
        friend class allocator;

        Strategy *strategy;


      public:
        using value_type = T;
        using strategy_type = Strategy;

        using propagate_on_container_copy_assignment = std::true_type;
        using propagate_on_container_move_assignment = std::true_type;
        using propagate_on_container_swap = std::true_type;


        /// ### Construct from a non-owning pointer to a strategy
        explicit allocator(Strategy &s) noexcept : strategy{&s} {}


        /// ### Converting constructor from allocator<U, Strategy>
        template<typename U>
        allocator(allocator<U, Strategy> const &other) noexcept
        : strategy{other.strategy} {}


        /// ### Allocate memory for `n` objects of type `T`
        [[nodiscard]] T *allocate(std::size_t const n) {
            return reinterpret_cast<T *>(strategy->allocate(n * sizeof(T)));
        }


        /// ### Deallocate memory for `n` objects of type `T`
        void deallocate(T *const p, std::size_t const n) noexcept {
            strategy->deallocate(p, n * sizeof(T));
        }


        /// ### Rebind to another type
        template<typename U>
        struct rebind {
            using other = allocator<U, Strategy>;
        };


        /// ### Equality comparison
        friend bool operator==(
                allocator const &lhs, allocator const &rhs) noexcept {
            return lhs.strategy == rhs.strategy;
        }

        friend bool operator!=(
                allocator const &lhs, allocator const &rhs) noexcept {
            return lhs.strategy != rhs.strategy;
        }
    };


}


template<typename T, typename S>
struct std::allocator_traits<felspar::memory::allocator<T, S>> {
    using allocator_type = felspar::memory::allocator<T, S>;
    using value_type = typename allocator_type::value_type;
    using pointer = value_type *;
    using const_pointer = value_type const *;
    using void_pointer = void *;
    using const_void_pointer = void const *;
    using difference_type = std::ptrdiff_t;
    using size_type = std::size_t;

    using propagate_on_container_copy_assignment =
            typename allocator_type::propagate_on_container_copy_assignment;
    using propagate_on_container_move_assignment =
            typename allocator_type::propagate_on_container_move_assignment;
    using propagate_on_container_swap =
            typename allocator_type::propagate_on_container_swap;

    template<typename U>
    using rebind_alloc = felspar::memory::allocator<U, S>;
    template<typename U>
    using rebind_traits = std::allocator_traits<rebind_alloc<U>>;

    [[nodiscard]] static pointer
            allocate(allocator_type &a, std::size_t const n) {
        return a.allocate(n);
    }

    static void deallocate(
            allocator_type &a, pointer p, std::size_t const n) noexcept {
        a.deallocate(p, n);
    }

    [[nodiscard]] static size_type max_size(allocator_type const &) noexcept {
        return std::numeric_limits<size_type>::max() / sizeof(value_type);
    }

    template<typename U, typename... Args>
    static void construct(allocator_type &, U *p, Args &&...args) {
        std::construct_at(p, std::forward<Args>(args)...);
    }


    template<typename U>
    static void destroy(allocator_type &, U *p) noexcept {
        std::destroy_at(p);
    }


    template<typename U>
    static felspar::memory::allocator<U, S>
            select_on_container_copy_construction(
                    felspar::memory::allocator<T, S> const &a) noexcept {
        return felspar::memory::allocator<U, S>{a};
    }
};
