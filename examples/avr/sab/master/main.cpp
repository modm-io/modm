
#include <xpcc/architecture/platform.hpp>
#include <xpcc/communication/sab/interface.hpp>

using namespace xpcc::atmega;

MAIN_FUNCTION
{
	xpcc::sab::Interface< Uart0 > interface;
	
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
