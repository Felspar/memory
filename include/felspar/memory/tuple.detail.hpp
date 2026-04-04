#pragma once


#include <utility>


namespace felspar::memory::detail {


    template<typename T, typename Tuple, std::size_t... Is>
    constexpr T construct_from_tuple(Tuple &&t, std::index_sequence<Is...>) {
        return T(std::get<Is>(std::forward<Tuple>(t))...);
    }


    template<typename T, typename Tuple>
    constexpr T construct_from_tuple(Tuple &&t) {
        constexpr auto size = std::tuple_size_v<std::remove_reference_t<Tuple>>;
        return construct_from_tuple<T>(
                std::forward<Tuple>(t), std::make_index_sequence<size>{});
    }


}
