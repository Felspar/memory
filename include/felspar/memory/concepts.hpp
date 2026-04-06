#pragma once


#include <felspar/memory/overallocation.hpp>

#include <utility>


namespace felspar::memory {


    /// ## A basic allocator
    template<typename S>
    concept allocator_strategy = requires(S strategy, std::size_t bytes) {
        { strategy.allocate(bytes) } -> std::same_as<std::byte *>;
        {
            strategy.deallocate(std::declval<void *>(), bytes)
        } -> std::same_as<void>;
    };


    /// ## An allocator that can refuse a request
    template<typename S>
    concept nullable_allocator_strategy =
            allocator_strategy<S> && requires(S strategy, std::size_t bytes) {
                { strategy.try_allocate(bytes) } -> std::same_as<std::byte *>;
            };


    /// ## An allocator able to track its usage
    template<typename S>
    concept owning_allocator_strategy = nullable_allocator_strategy<S>
            && requires(S strategy, void const *p) {
                   { strategy.owns(p) } noexcept -> std::same_as<bool>;
               };


    /// ## An allocator that can return more memory than requested
    /**
     * Strategies satisfying this concept may allocate more bytes than asked for
     * and report the true size to the caller. Extends `allocator_strategy`
     * orthogonally — independent of the `nullable_` and `owning_` chains.
     *
     * The canonical example is a fixed-block pool: every allocation consumes a
     * full block, so the strategy can always report the true block size.
     */
    template<typename S>
    concept overallocating_allocator_strategy =
            allocator_strategy<S> && requires(S strategy, std::size_t bytes) {
                {
                    strategy.allocate_at_least(bytes)
                } -> std::same_as<allocation_result>;
                {
                    strategy.try_allocate_at_least(bytes)
                } -> std::same_as<allocation_result>;
            };


}
