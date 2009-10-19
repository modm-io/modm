
#include <stdint.h>

#include <src/math/filter/ramp/ramp.hpp>

using namespace xpcc;

volatile uint8_t out;

int
main(void)
{
	Ramp<uint8_t> filter(4,5);

	filter.update(20);
	out = filter.getValue();
	
	filter.update(20);
	out = filter.getValue();
	
	filter.update(20);
	out = filter.getValue();
	
	filter.update(0);
	out = filter.getValue();
	
	while(1) {
		
	}
}

