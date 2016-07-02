#include <xpcc/architecture/platform.hpp>
using namespace Board;
using namespace xpcc::glcd;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::initializeDisplay();

	xpcc::GraphicDisplay& display = Board::getDisplay();
	display.setFont(xpcc::font::Assertion);

	display.drawRectangle(0,0, 10, 10);
	display.drawRectangle(0,470, 10, 10);
	display.drawRectangle(790,0, 10, 10);
	display.drawRectangle(790,470, 10, 10);

	display.drawRectangle(-1,-1, 10, 10);
	display.drawRectangle(-1,471, 10, 10);
	display.drawRectangle(791, -1, 10, 10);
	display.drawRectangle(791,471, 10, 10);

	while (1)
	{
		xpcc::delayMilliseconds(50);

		if (Button::read()) {
			display.clear();
		} else {
			display.setCursor(rand() % 750, rand() % 460);
			display << "Hello World!";
			display.setColor(Color(uint16_t(rand())));
		}
	}

	return 0;
}
