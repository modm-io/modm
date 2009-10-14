
#include <avr/io.h>
#include "../../src/math/polar_coordinate.hpp"
#include "../../src/math/position.hpp"

using namespace xpcc;

PolarCoordinate<int16_t> bar;

int
main(void)
{
	PORTB = bar.getLength();
	
	Position foo = bar.toCartesian();
	
	while (1) {
		
	}
}
