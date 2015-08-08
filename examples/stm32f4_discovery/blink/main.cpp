#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

uint16_t buffer[] ATTRIBUTE_FASTDATA = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 900, 800, 700, 600, 500, 400, 300, 200};

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedOrange::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);
	LedRed::setOutput(xpcc::Gpio::High);
	LedBlue::setOutput(xpcc::Gpio::High);

	int counter(0);

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		xpcc::delayMilliseconds(buffer[counter]);
		counter++;
		counter %= XPCC__ARRAY_SIZE(buffer);
	}

	return 0;
}
