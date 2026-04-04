#pragma once

#include <cstddef>
#include <utility>


namespace felspar::memory {


    template<typename S>
    concept allocator_strategy = requires(S strategy, std::size_t bytes) {
        { strategy.allocate(bytes) } -> std::same_as<std::byte *>;
        {
            strategy.deallocate(std::declval<void *>(), bytes)
        } -> std::same_as<void>;
    };


    template<typename S>
    concept nullable_allocator_strategy =
            allocator_strategy<S> && requires(S strategy, std::size_t bytes) {
                { strategy.try_allocate(bytes) } -> std::same_as<std::byte *>;
            };


    template<typename S>
    concept owning_allocator_strategy = nullable_allocator_strategy<S>
            && requires(S strategy, void const *p) {
                   { strategy.owns(p) } noexcept -> std::same_as<bool>;
               };


}
