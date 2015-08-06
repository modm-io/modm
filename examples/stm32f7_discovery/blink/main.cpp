#include <xpcc/architecture/platform.hpp>
#include "../stm32f7_discovery.hpp"

// ----------------------------------------------------------------------------
MAIN_FUNCTION
{
//	defaultSystemClock::enable();

	while (1)
	{
		xpcc::delayMilliseconds(500);
	}

	return 0;
}
