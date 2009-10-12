
#include <avr/io.h>

#include "../../../src/math/filter/moving_average/moving_average.hpp"

using namespace xpcc;

int main(void)
{
	MovingAverage<uint8_t, 4> filter(10);

	filter.update(20);
	PORTB = filter.getValue();
	
	filter.update(20);
	PORTB = filter.getValue();
	
	filter.update(20);
	PORTB = filter.getValue();
	
	filter.update(20);
	PORTB = filter.getValue();
	
	while(1) {
		
	}
}

