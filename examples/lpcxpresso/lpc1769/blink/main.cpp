
#include <xpcc/architecture.hpp>

GPIO__OUTPUT(Led, 0, 22);

int
main(void)
{
	SystemInit();
	
	// Set LED port pin to output
	Led::setOutput();
	
	while (1)
	{
		Led::reset();
		xpcc::delayMilliseconds(100);
		Led::set();
		xpcc::delayMilliseconds(100);
	}
}
