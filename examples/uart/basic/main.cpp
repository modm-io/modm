
#include <avr/interrupt.h>
#include <xpcc/architecture/atmega/uart.hpp>

// Create a new UART object and configure it to a baudrate of 115200
xpcc::BufferedUart0 uart(115200);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Write the unavoidable standard message
	uart.write("Hello World!\n");
	
	while (1)
	{
	}
}
