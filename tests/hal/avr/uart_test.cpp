
#include <avr/io.h>
#include <avr/interrupt.h>

#include "../../../src/hal/avr/uart.hpp"
#include "../../../src/hal/io/iostream.hpp"

int main(void)
{
	DDRB = (1 << PB0) | (1 << PB1);
	PORTB |= (1 << PB0);
	
	//xpcc::Uart& uart = xpcc::Uart::instance();
	xpcc::Uart uart;
	
	sei();
	
	uart.initialize(UART_BAUD_SELECT(9600UL, F_CPU));
	uart.put('a');
	uart.put('\n');
	uart.put("Hallo Welt!\n");
	
	PORTB |= (1 << PB1);
	
	xpcc::IOStream stream(uart);
	
	stream << static_cast<uint8_t>(0xff) << ' ' 
		   << static_cast<int16_t>(3) << xpcc::endl;
	stream << 10 << xpcc::endl;
	stream << 2.2 << xpcc::endl;
	stream << "Hallo" << xpcc::endl;
	
	while (1) {
		
	}
	
	return 0;
}
