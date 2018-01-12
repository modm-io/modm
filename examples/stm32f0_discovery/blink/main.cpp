#include <modm/board/board.hpp>

using namespace Board;

int
main()
{
	Board::initialize();

	LedGreen::set();
	LedBlue::reset();

	while (true) {
		LedGreen::toggle();
		LedBlue::toggle();
		modm::delayMilliseconds(Board::Button::read() ? 500 : 1000);
	}
}
