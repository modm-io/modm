
// this header will include the right header for our architecture
#include <xpcc/architecture/general/gpio.hpp>

// create a wrapper for a output pin
GPIO__OUTPUT(LED, B, 0);

int
main(void)
{
	LED::setOutput();
	LED::set();
	
	while (1)
	{
	}
}
