#include "sort.hpp"
#include <tuple>

int main() {
    constexpr std::tuple<int, int, char, double> s{1, 2, 'a', 3.14};
    constexpr std::tuple<int, int, char, double> s1{2, 1, 'a', 3.14};

    static_assert(sizedSort(s) == std::tuple<char, int, int, double>{'a', 1, 2, 3.14});
    static_assert(sizedSort(s1) == std::tuple<char, int, int, double>{'a', 2, 1, 3.14});

    return 0;
}