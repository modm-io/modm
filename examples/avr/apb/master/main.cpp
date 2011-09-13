
#include <xpcc/architecture/platform.hpp>
#include <xpcc/driver/connectivity/apb/interface.hpp>

MAIN_FUNCTION
{
	xpcc::apb::Interface< xpcc::atmega::BufferedUart0 > interface;
	
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
