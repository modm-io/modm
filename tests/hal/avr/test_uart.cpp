
#include <avr/io.h>
#include <avr/interrupt.h>

#include <xpcc/architecture/avr/atmega/uart/uart0.hpp>
#include <xpcc/io/iodevice_wrapper.hpp>
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
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
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
