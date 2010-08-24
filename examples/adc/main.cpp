
#include <xpcc/architecture/platform.hpp>
#include <xpcc/architecture/driver/atmega/adc.hpp>
#include <xpcc/architecture/driver/atmega/uart.hpp>
#include <xpcc/architecture/driver/time.hpp>

#include <xpcc/io/iostream.hpp>

// Create a new UART object and configure it to a baudrate of 9600
xpcc::BufferedUart0 uart(9600);

int
main()
{
	// Enable interrupts, this is needed for every buffered UART
	sei();
	
	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper<xpcc::BufferedUart0> device(uart);
	xpcc::IOStream output(device);
	
	output << "Welcome!" << xpcc::endl;
	
	// Initialize the analog to digital converter
	// With the AVR running at 14.7456Mhz and a prescaler of 128 the
	// ADC is running at 115kHz.
	xpcc::Adc::initialize(
			xpcc::Adc::REFERENCE_INTERNAL_AVCC,
			xpcc::Adc::PRESCALER_128);
	
	// read the value of channel 0 (=> ADC0 => PA0) and wait until
	// it is finished
	uint16_t value = xpcc::Adc::readChannel(0);
	
	// send it via UART
	output << value << xpcc::endl;
	
	// now lets use the asynchronous version the be able to do other stuff
	// while waiting for the result
	xpcc::Adc::startConversion(0);
	
	while (1)
	{
		if (xpcc::Adc::isFinished())
		{
			// send the result
			output << xpcc::Adc::getValue() << xpcc::endl;
			
			// wait a bit so that the result stays readable when
			// receiving the UART output on a PC. If you want to do further
			// calculations with the results instead of sending it
			// you do not need this.
			xpcc::delay_ms(300);
			
			// restart the conversion
			xpcc::Adc::startConversion(0);
		}
	}
}
