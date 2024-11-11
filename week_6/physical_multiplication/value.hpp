#ifndef __VALUE_HPP__
#define __VALUE_HPP__

#include <utility>
#include <iostream>
#include <functional>

namespace Physical {

template<int... Ints> struct Unit {
	using type = std::integer_sequence<int, Ints...>;
	// static_assert(type::size() == 3);
};

template<int... Ints>
consteval auto createType(std::integer_sequence<int, Ints...> a) {
	return Unit<Ints...>{};
}

template<class Functor, class Dim1, class Dim2>
struct Transform;

template<class Functor, int... Dims1, int... Dims2>
struct Transform<
	Functor,
	std::integer_sequence<int, Dims1...>, 
	std::integer_sequence<int, Dims2...>
> {
	static_assert(sizeof...(Dims1)==sizeof...(Dims2), "wrong dimensions");
	static constexpr Functor f{};
	using type=std::integer_sequence<int, f(Dims1,Dims2)...>;
};

template <typename Op, typename D1, typename D2>
constexpr auto map(Op op, D1 a, D2 b) {
	using D = Transform<Op, typename D1::type, typename D2::type>::type;
	return createType(D{});
}

template<typename Unit> struct Value {
	double val;
	explicit constexpr Value(double d) : val(d) {}
	explicit constexpr operator double() const { return val; }

	Value &operator*=(double coef) {
		val *= coef;
		return *this;
	}

	Value &operator/=(double coef) {
		val /= coef;
		return *this;
	}
};

using Meter = Unit<1,0,0>;
using Meter2 = Unit<2,0,0>;
using Second = Unit<0,0,1>;
using Second2 = Unit<0,0,2>;
using Scalar = Value<Unit<0,0,0>>;
using Length = Value<Unit<1,0,0>>;
using Speed = Value<Unit<1,0,-1>>;
using Acceleration = Value<Unit<1,0,-2>>;

constexpr Value<Meter> operator"" _m(long double d) { return Value<Meter>(d); }
constexpr Value<Meter2> operator"" _m2(long double d) { return Value<Meter2>(d); }
constexpr Value<Second> operator"" _s(long double d) { return Value<Second>(d); }
constexpr Value<Second2> operator"" _s2(long double d) { return Value<Second2>(d); }

template <typename D1, typename D2>
auto operator*(const Value<D1> &a, const Value<D2> &b) {
	using D = decltype(map(std::plus<>{}, D1{}, D2{}));
	return Value<D>{double(a) * double(b)};
}

template <typename D1, typename D2>
auto operator/(const Value<D1> &a, const Value<D2> &b) {
	using D = decltype(map(std::minus<>{}, D1{}, D2{}));
	return Value<D>{double(a) / double(b)};
}

// multiplication with scalar
template <typename D>
auto operator*(const Value<D> &val, double coef) {
	Value<D> tmp = val;
	tmp *= coef;
	return tmp;
}

template <typename D>
auto operator*(double coef, const Value<D> &val) {
	return val*coef;
}

// division with scalar
template <typename D>
auto operator/(const Value<D> &val, double coef) {
	Value<D> tmp = val;
	tmp /= coef;
	return tmp;
}

template <typename D>
auto operator/(double a, const Value<D> &b) {
	Scalar tmp = a;
	return tmp / b;
}

}

#endif