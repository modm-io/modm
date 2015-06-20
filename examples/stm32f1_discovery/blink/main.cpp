#include <xpcc/architecture.hpp>
#include "../stm32f1_discovery.hpp"

MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedLeft::setOutput(xpcc::Gpio::Low);
	LedRight::setOutput(xpcc::Gpio::High);

	while (1)
	{
		LedLeft::toggle();
		xpcc::delayMilliseconds(100);
		LedRight::toggle();
		xpcc::delayMilliseconds(100);
	}

	return 0;
}
