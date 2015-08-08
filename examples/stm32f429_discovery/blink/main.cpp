#include <xpcc/architecture/platform.hpp>
#include "../stm32f429_discovery.hpp"

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedRed::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);

	VBusPresent::setOutput(xpcc::Gpio::Low);
	VBusOvercurrent::setOutput(xpcc::Gpio::Low);

	Button::setInput();

	while (1)
	{
		LedRed::toggle();
		LedGreen::toggle();

		xpcc::delayMilliseconds(Button::read() ? 125 : 500);

		VBusPresent::toggle();
		VBusOvercurrent::toggle();

		xpcc::delayMilliseconds(Button::read() ? 125 : 500);
	}

	return 0;
}
