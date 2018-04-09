#include <modm/board/board.hpp>

using namespace Board;

/*
 * Blinks the orange user LED with 1 Hz.
 * It is on for 90% of the time and off for 10% of the time.
 */

int
main()
{
	Board::initialize();

	LedOrange::set();

	while (true)
	{
		LedOrange::set();
		modm::delayMilliseconds(900);

		LedOrange::reset();
		modm::delayMilliseconds(100);
	}

	return 0;
}
