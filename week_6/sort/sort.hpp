#ifndef __SORT_HPP__
#define __SORT_HPP__

#include <tuple>
#include <array>

template<std::size_t I>
using index_c = std::integral_constant<std::size_t, I>;

template<typename... Args, std::size_t... Is>
constexpr auto get_index_order(const std::tuple<Args...> &tup, const std::index_sequence<Is...> &is) {
    std::array<std::size_t, sizeof...(Is)> indices{Is...};

    auto do_swap = [&]<std::size_t I, std::size_t J>(index_c<I>, index_c<J>) {
        if (J <= I) return;
        if (sizeof(decltype(std::get<I>(tup))) <= sizeof(decltype(std::get<J>(tup)))) return;

        std::swap(indices[I], indices[J]);
    };

    auto swap_with_min = [&]<std::size_t I, std::size_t... Js>(index_c<I> i, std::index_sequence<Js...>) {
        (do_swap(i, index_c<Js>{}), ...);
    };

    (swap_with_min(index_c<Is>{}, is), ...);

    return indices;
}

template<typename... Args>
constexpr auto sizedSort(std::tuple<Args...> s) {
    constexpr auto idx = std::index_sequence_for<Args...>{};
    constexpr auto arr = get_index_order(decltype(s){}, idx);

    auto to_sequence = [arr]<std::size_t... Is>(std::index_sequence<Is...> is) {
        return std::integer_sequence<std::size_t, arr[Is]...>{};
    };
    auto createTuple = [s] <std::size_t... Is>(std::integer_sequence<std::size_t, Is...> is) {
        return std::make_tuple(std::get<Is>(s)...);
    };

    return createTuple(to_sequence(idx));
}

#endif