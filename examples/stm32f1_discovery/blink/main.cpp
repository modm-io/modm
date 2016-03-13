#include <xpcc/architecture/platform.hpp>

using namespace Board;

int
main()
{
	Board::initialize();

	LedBlue::set();

	while (1)
	{
		LedGreen::toggle();
		xpcc::delayMilliseconds(Button::read() ? 100 : 300);

		LedBlue::toggle();
		xpcc::delayMilliseconds(Button::read() ? 100 : 300);
	}

	return 0;
}
