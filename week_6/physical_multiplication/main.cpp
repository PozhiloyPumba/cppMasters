#include "value.hpp"
#include <functional>

int main() {
	using namespace Physical;
	Speed sp1 = 100._m/9.8_s;
	// Speed sp2 = 100._m/9.8_s2; // error
	Value<Unit<0,0,-1>> sp3 = 100./9.8_s;
	Acceleration acc1 = sp1/0.5_s;

	return 0;
}