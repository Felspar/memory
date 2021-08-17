#pragma once


#include <type_traits>


/// libc++ doesn't have a concepts header yet, so these are just basic
/// implementations whilst we wait for it to catch up to libstdc++
namespace felspar {


    template<typename T, typename U>
    concept same_as = std::is_same_v<T, U> and std::is_same_v<U, T>;


    template<class LHS, class RHS>
    concept assignable_from = requires(LHS lhs, RHS &&rhs) {
        { lhs = std::forward<RHS>(rhs) } -> same_as<LHS>;
    };


}
