
// this header will include the right header for our architecture
#include <xpcc/architecture/driver/gpio.hpp>

// create a wrapper for a output pin
GPIO__OUTPUT(LED, B, 0);
GPIO__OUTPUT(LED2, B, 1);

typedef xpcc::gpio::Invert< LED2 > LedInverted;

int
main(void)
{
	LED::setOutput();
	LED::set();
	
	LedInverted::setOutput();
	LedInverted::reset();
	
	while (1)
	{
	}
}
