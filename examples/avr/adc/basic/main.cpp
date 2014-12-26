
#include <xpcc/architecture.hpp>

#include <xpcc/io/iostream.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;
xpcc::atmega::Uart0 uart;


int
main()
{
    // Create a new UART object and configure it to a baudrate of 9600
    GpioOutputD1::connect(Uart0::Tx);
    GpioInputD0::connect(Uart0::Rx);
    uart.initialize<clock, 115200>();

    // Enable interrupts, this is needed for every buffered UART
	sei();

	// Create a IOStream for complex formatting tasks
	xpcc::IODeviceWrapper< Uart0, xpcc::IOBuffer::BlockIfFull > device(uart);
	xpcc::IOStream output(device);

	output << "Welcome!" << xpcc::endl;

	// Initialize the analog to digital converter
	// With the AVR running at 14.7456Mhz and a prescaler of 128 the
	// ADC is running at 115kHz.
	Adc::initialize(
			Adc::Reference::InternalAVcc,
			Adc::Prescaler::Div128);

	// read the value of channel 0 (=> ADC0 => PA0) and wait until
	// it is finished
	uint16_t value = Adc::readChannel(0);

	// send it via UART
	output << value << xpcc::endl;

	// now lets use the asynchronous version the be able to do other stuff
	// while waiting for the result
	Adc::startConversion(0);

	while (1)
	{
		if (Adc::isFinished())
		{
			// send the result
			output << Adc::getValue() << xpcc::endl;

			// wait a bit so that the result stays readable when
			// receiving the UART output on a PC. If you want to do further
			// calculations with the results instead of sending it
			// you do not need this.
			xpcc::delayMilliseconds(300);

			// restart the conversion
			Adc::startConversion(0);
		}
	}
}
