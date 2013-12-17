#include <xpcc/architecture.hpp>
#include "../stm32f3_discovery.hpp"

using namespace xpcc::stm32;

MAIN_FUNCTION
{
	typedef Pll<ExternalClock<MHz8>, MHz72> clockSource;
	StartupError err =
		SystemClock<clockSource>::enable();

	LedNorth::setOutput(xpcc::Gpio::LOW);
	LedNorthEast::setOutput(xpcc::Gpio::HIGH);
	LedEast::setOutput(xpcc::Gpio::HIGH);
	LedSouthEast::setOutput(xpcc::Gpio::HIGH);
	LedSouth::setOutput(xpcc::Gpio::HIGH);
	LedSouthWest::setOutput(xpcc::Gpio::HIGH);
	LedWest::setOutput(xpcc::Gpio::HIGH);
	LedNorthWest::setOutput(xpcc::Gpio::HIGH);

	// Output SystemClock on PA8
	ClockOut::setOutput();
	ClockOut::connect(MCO::Id);
	MCO::connect(clockSource::Id);

	while (1)
	{
		LedNorth::toggle();
		xpcc::delay_ms(100);
		LedNorthEast::toggle();
		xpcc::delay_ms(100);
		LedEast::toggle();
		xpcc::delay_ms(100);
		LedSouthEast::toggle();
		xpcc::delay_ms(100);
		LedSouth::toggle();
		xpcc::delay_ms(100);
		LedSouthWest::toggle();
		xpcc::delay_ms(100);
		LedWest::toggle();
		xpcc::delay_ms(100);
		LedNorthWest::toggle();
		xpcc::delay_ms(100);
	}

	return 0;
}
