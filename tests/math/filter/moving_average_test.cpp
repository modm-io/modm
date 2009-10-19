
#include "../../../src/math/filter/moving_average/moving_average.hpp"

using namespace xpcc;

volatile uint8_t out;

int main(void)
{
	MovingAverage<uint8_t, 4> filter(10);

	filter.update(20);
	out = filter.getValue();
	
	filter.update(20);
	out = filter.getValue();
	
	filter.update(20);
	out = filter.getValue();
	
	filter.update(20);
	out = filter.getValue();
	
	while(1) {
		
	}
}

