
#include <avr/io.h>
#include <avr/interrupt.h>

#include <xpcc/hal/peripheral/atmega/uart/buffered_uart0.hpp>
#include <xpcc/hal/peripheral/atmega/uart/uart_device.hpp>
#include <xpcc/io/iostream.hpp>

int main(void)
{
	xpcc::BufferedUart0 uart(9600);
	
	// enable interrupts
	sei();
	
	// try sending single chars
	uart.put('a');
	xpcc::BufferedUart0::put('\n');
	
	// create a generic device from the uart
	xpcc::UartDevice<xpcc::BufferedUart0> device(uart);
	device.put("Hallo Welt!\n");
	
	// test a stream
	xpcc::IOStream stream(device);
	
	stream << static_cast<uint8_t>(0xff) << ' ' 
		   << static_cast<int16_t>(3) << xpcc::endl;
	stream << 10 << xpcc::endl;
	stream << 2.2 << xpcc::endl;
	stream << "Hallo" << xpcc::endl;
	
	while (1)
	{
	}
}
