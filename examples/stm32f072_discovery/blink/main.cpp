#include "../stm32f072_discovery.hpp"

using namespace Board;

MAIN_FUNCTION
{
	Board::initialize();

	LedUp::set();
	LedDown::set();

	while (1) {
		LedRight::toggle();
		LedUp::toggle();
		LedLeft::toggle();
		LedDown::toggle();
		xpcc::delayMilliseconds(1000);
	}
}
