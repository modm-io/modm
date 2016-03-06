#include "../stm32f4_discovery.hpp"

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	LedOrange::set();
	LedRed::set();

	while (1)
	{
		LedBlue::toggle();
		LedGreen::toggle();
		LedOrange::toggle();
		LedRed::toggle();
		xpcc::delayMilliseconds(Button::read() ? 250 : 500);
	}

	return 0;
}
