
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
		xpcc::delay_ms(100);
		Led::set();
		xpcc::delay_ms(100);
	}
}
