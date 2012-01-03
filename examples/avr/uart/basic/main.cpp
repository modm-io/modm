
#include <xpcc/architecture/platform.hpp>

// Create a new UART object and configure it to a baudrate of 115200
xpcc::atmega::BufferedUart0 uart(115200);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Write some characters
	uart.write('H');
	uart.write('e');
	uart.write('l');
	uart.write('l');
	uart.write('o');
	uart.write('\n');
	
	while (1)
	{
	}
}
