#pragma once


#include <felspar/concepts.hpp>

#include <compare>
#include <span>


namespace felspar::memory {


    template<numeric T>
    auto spaceship(
            std::span<T const> const left, std::span<T const> const right) {
        for (typename std::span<T>::size_type index{}; true; ++index) {
            if (index == left.size()) {
                if (index < right.size()) {
                    return std::strong_ordering::less;
                } else {
                    return std::strong_ordering::equal;
                }
            } else if (index == right.size()) {
                return std::strong_ordering::greater;
            } else if (left[index] < right[index]) {
                return std::strong_ordering::less;
            } else if (left[index] > right[index]) {
                return std::strong_ordering::greater;
            }
        }
    }


}
