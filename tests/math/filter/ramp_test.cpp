
#include <avr/io.h>

#include "../../../src/math/filter/ramp/ramp.hpp"

using namespace xpcc;

int main(void)
{
	Ramp<uint8_t> filter(4,5);

	filter.update(20);
	PORTB = filter.getValue();
	
	filter.update(20);
	PORTB = filter.getValue();
	
	filter.update(20);
	PORTB = filter.getValue();
	
	filter.update(0);
	PORTB = filter.getValue();
	
	while(1) {
		
	}
}

