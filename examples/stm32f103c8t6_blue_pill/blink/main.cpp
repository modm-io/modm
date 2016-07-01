#include <xpcc/architecture/platform.hpp>

using namespace Board;

/*
 * Blinks the green user LED with 1 Hz.
 * It is on for 90% of the time and off for 10% of the time.
 */

int
main()
{
	Board::initialize();

	LedGreen::set();

	while (true)
	{
		LedGreen::set();
		xpcc::delayMilliseconds(900);

		LedGreen::reset();
		xpcc::delayMilliseconds(100);
	}

	return 0;
}
