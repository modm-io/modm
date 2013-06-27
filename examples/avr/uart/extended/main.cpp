
#include <xpcc/architecture/platform.hpp>
#include <xpcc/io/iostream.hpp>

// Create a new UART object and configure it to a baudrate of 115200
xpcc::atmega::Uart0 uart(115200);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::atmega::Uart0> device(uart);
	xpcc::IOStream stream(device);
	
	// Now we can print numbers and other objects to the stream
	// The following will result in the string "24 is a nice number!\n" with
	// the number formatted as ASCII text.
	stream << 24 << " is a nice number!" << xpcc::endl;
	
	while (1)
	{
	}
}
