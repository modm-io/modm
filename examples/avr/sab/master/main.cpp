
#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/connectivity/sab/interface.hpp>

MAIN_FUNCTION
{
	xpcc::sab::Interface< xpcc::atmega::BufferedUart0 > interface;
	
	// set baudrate etc.
	interface.initialize();
	
	// enable interrupts
	sei();
	
	while(1)
	{
		// decode received messages
		interface.update();
		
		// ... TODO ...
	}
}
