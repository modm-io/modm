
#include <avr/interrupt.h>

#include <xpcc/architecture/atmega/uart/uart0.hpp>
#include <xpcc/driver/apb/interface.hpp>

MAIN_FUNCTION
{
	xpcc::apb::Interface< xpcc::BufferedUart0 > interface;
	
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
