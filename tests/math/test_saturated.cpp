
#include <avr/io.h>
#include <xpcc/math/saturated.hpp>

using namespace xpcc;

Saturated<int8_t> a;
Saturated<int8_t> b(125);
Saturated<int8_t> c;

volatile int8_t test;

int
main(void)
{
	a += Saturated<int8_t>(12);
	
	c = a + b;
	
	test = c.getValue();
	
	while (1) {
		
	}
}
