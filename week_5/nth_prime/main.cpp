#include <iostream>
#include <type_traits>

template <int N, int L = 1, int H = N, int mid = (L + H + 1) / 2>
struct Sqrt : std::integral_constant<int,
    std::conditional_t<(N < mid * mid),
    Sqrt<N, L, mid - 1>,
    Sqrt<N, mid, H> >{}> {};

template <int N, int S> 
struct Sqrt <N, S, S, S> : std::integral_constant<int, S> {};

namespace impl {
    template<int n, int i>
    struct is_prime :
        std::conditional<(n % i) == 0,
        std::false_type, is_prime<n, i - 1>>::type {};
    
    template<int n>
    struct is_prime<n, 1> : std::true_type{};
}

template<int n>
struct is_prime : impl::is_prime<n, Sqrt<n>::value>{};

namespace impl {
    template<unsigned n>
    struct Nth_prime;
	
	template<unsigned n>
    struct NextShift : std::integral_constant<unsigned, (n % 6 + 3) % 6>{};

    template<unsigned n, int shift>
    struct Nth_prime_shifted : 
        std::integral_constant<unsigned, 
            std::conditional_t<
                ::is_prime<Nth_prime<n - 1>::value + shift>::value,
                std::integral_constant<unsigned, Nth_prime<n - 1>::value + shift>,
				Nth_prime_shifted<n, shift + NextShift<Nth_prime<n - 1>::value + shift>::value>
            >{}
        >{};

    template<unsigned n>
    struct Nth_prime : Nth_prime_shifted<n, NextShift<Nth_prime<n - 1>::value>::value>{};

    template<> struct Nth_prime<3>: std::integral_constant<unsigned, 5>{};
    template<> struct Nth_prime<2>: std::integral_constant<unsigned, 3>{};
    template<> struct Nth_prime<1>: std::integral_constant<unsigned, 2>{};
    template<> struct Nth_prime<0>;
}

template<unsigned n>
struct Nth_prime : impl::Nth_prime<n>{};

int main() {
    static_assert(Nth_prime<100>::value == 541);
    static_assert(Nth_prime<10>::value == 29);
    static_assert(Nth_prime<1>::value == 2);
    static_assert(Nth_prime<2>::value == 3);
    static_assert(Nth_prime<3>::value == 5);
    return 0;
}