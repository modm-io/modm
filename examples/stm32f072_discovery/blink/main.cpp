#include <xpcc/architecture/platform.hpp>

using namespace Board;

int
main()
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
