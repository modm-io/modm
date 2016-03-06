#include <xpcc/architecture.hpp>
#include "../stm32f4_loa_v2b.hpp"

int
main()
{
	defaultSystemClock::enable();

	LedWhite::setOutput(xpcc::Gpio::High);
	LedGreen::setOutput(xpcc::Gpio::Low);

	while (1)
	{
		LedWhite::toggle();
		LedGreen::toggle();
		xpcc::delayMilliseconds(100);
	}

	return 0;
}
