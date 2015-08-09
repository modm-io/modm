#include "../stm32f3_discovery.hpp"
#include <xpcc/driver/usb/ft245.hpp>

/**
 * Example of FT245 usage.
 *
 * Connect D0 to D7 of FT245 to PD0 to PD7.
 * Connect Rxf, Txe, Rd and Wr to their corresponding pins.
 * Start a terminal and connect to the virtual serial port of the FT245.
 * The microcontroller echos the received character increased by one,
 * e.g. send 'a' and receive 'b' from the microcontroller.
 * For every character received the red LED toggles once.
 *
 */
MAIN_FUNCTION
{
	Board::initialize();

	typedef xpcc::stm32::GpioPort<xpcc::stm32::GpioD0, 8> myPort;
	typedef xpcc::stm32::GpioInputC11  Rxf;
	typedef xpcc::stm32::GpioInputC10  Txe;
	typedef xpcc::stm32::GpioOutputA15 Rd;
	typedef xpcc::stm32::GpioOutputA9  Wr;

	Rd::setOutput(xpcc::Gpio::High);
	Wr::setOutput(xpcc::Gpio::Low);

	typedef xpcc::Ft245<myPort, Rd, Wr, Rxf, Txe> myFt;

	while (1)
	{
		uint8_t c;
		if (myFt::read(c)) {
			myFt::write(c + 1);
			Board::LedNorth::toggle();
		}
	}

	return 0;
}
