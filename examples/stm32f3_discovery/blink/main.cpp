#include <xpcc/architecture.hpp>
#include "../stm32f3_discovery.hpp"

MAIN_FUNCTION
{
	defaultSystemClock::enable();

	LedNorth::setOutput(xpcc::Gpio::Low);
	LedNorthEast::setOutput(xpcc::Gpio::High);
	LedEast::setOutput(xpcc::Gpio::High);
	LedSouthEast::setOutput(xpcc::Gpio::High);
	LedSouth::setOutput(xpcc::Gpio::High);
	LedSouthWest::setOutput(xpcc::Gpio::High);
	LedWest::setOutput(xpcc::Gpio::High);
	LedNorthWest::setOutput(xpcc::Gpio::High);

	while (1)
	{
		LedNorth::toggle();
		xpcc::delayMilliseconds(100);
		LedNorthEast::toggle();
		xpcc::delayMilliseconds(100);
		LedEast::toggle();
		xpcc::delayMilliseconds(100);
		LedSouthEast::toggle();
		xpcc::delayMilliseconds(100);
		LedSouth::toggle();
		xpcc::delayMilliseconds(100);
		LedSouthWest::toggle();
		xpcc::delayMilliseconds(100);
		LedWest::toggle();
		xpcc::delayMilliseconds(100);
		LedNorthWest::toggle();
		xpcc::delayMilliseconds(100);
	}

	return 0;
}
