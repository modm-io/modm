
#include <avr/interrupt.h>
#include <xpcc/architecture/atmega/uart.hpp>

#include <xpcc/io/iostream.hpp>

// Create a new UART object and configure it to a baudrate of 115200
xpcc::BufferedUart0 uart(115200);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	xpcc::IOStream stream(device);
	
	stream << 24 << " is a nice number!" << xpcc::endl;
	
	while (1)
	{
	}
}
