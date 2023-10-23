#pragma once


#include <type_traits>
#include <utility>


/// libc++ doesn't have a concepts header yet, so these are just basic
/// implementations whilst we wait for it to catch up to libstdc++
namespace felspar {


    template<typename T, typename U>
    concept same_as = std::is_same_v<T, U> and std::is_same_v<U, T>;


    template<class LHS, class RHS>
    concept assignable_from = requires(LHS lhs, RHS &&rhs) {
        { lhs = std::forward<RHS>(rhs) } -> same_as<LHS>;
    };


    template<typename N>
    concept numeric = std::is_integral_v<N> or std::is_floating_point_v<N>
            or std::is_pointer_v<N>;


}
