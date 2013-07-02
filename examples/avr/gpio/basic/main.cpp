
// this header will include the right header for our architecture
#include <xpcc/architecture.hpp>

// create a wrapper for a output pin
GPIO__OUTPUT(Led, B, 0);
GPIO__OUTPUT(Led2, B, 1);

typedef xpcc::GpioInverted< Led2 > LedInverted;

int
main(void)
{
	Led::setOutput();
	Led::set();
	
	LedInverted::setOutput();
	LedInverted::reset();
	
	while (1)
	{
	}
}
