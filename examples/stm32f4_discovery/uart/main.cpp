#include <xpcc/architecture.hpp>
#include "../stm32f4_discovery.hpp"

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	// Enable USART 2
	GpioOutputA2::connect(Usart2::Tx);
	GpioInputA3::connect(Usart2::Rx, Gpio::InputType::PullUp);
	Usart2::initialize<defaultSystemClock, 9600>(12);

	while (1)
	{
		static uint8_t c = 'A';
		LedRed::toggle();
		LedGreen::toggle();
		Usart2::write(c);
		++c;
		if (c == 'Z' + 1) {
			c = 'A';
		}
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
