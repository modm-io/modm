#include <modm/board/board.hpp>

using namespace Board;

// ----------------------------------------------------------------------------
int
main()
{
	initialize();

	LedRed::set();

	while (true)
	{
		LedGreen::toggle();
		LedRed::toggle();
		modm::delayMilliseconds(Button::read() ? 250 : 500);
	}

	return 0;
}
