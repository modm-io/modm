#include <xpcc/architecture/platform.hpp>
#include "../stm32f4_discovery.hpp"

uint32_t buffer[100] __attribute__((section(".ccm"))) = {100, 200, 300, 400, 500, 600, 700, 800, 900, 1000, 1100, 1200, 1300, 1400, 1500, 1400, 1300, 1200, 1100, 1000, 900, 800, 700, 600, 500, 400, 300, 200, 100};

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
		counter %= 29;
	}

	return 0;
}
