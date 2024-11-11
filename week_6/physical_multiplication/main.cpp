#include "value.hpp"
#include <functional>

int main() {
	using namespace Physical;
	Speed sp1 = 100._m/9.8_s; // ok
	// Speed sp2 = 100._m/9.8_s2; // error
	// Value<Unit<0,0,-1>> sp3 = 100./9.8_s; // error
	Acceleration acc1 = sp1/0.5_s;

	auto a = map([](auto a, auto b){ return a-b;}, Unit<-1>{}, Unit<0>{});
	static_assert(std::is_same_v<decltype(a), Unit<-1>>);
	return 0;
}