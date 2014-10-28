
#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>
#include <xpcc/ui/button_group.hpp>

using namespace xpcc::atmega;

typedef GpioOutputA0 Mosi;
typedef GpioOutputA1 Sck;
typedef xpcc::SoftwareSpiSimpleMaster<Sck, Mosi, xpcc::GpioUnused> SPI;

typedef GpioOutputA2 Cs;
typedef GpioOutputA3 Rs;
typedef GpioOutputA4 Reset;
typedef xpcc::SiemensS65Portrait<SPI, Cs, Rs, Reset> Display;

typedef GpioOutputA5 Backlight;

MAIN_FUNCTION
{
	Display display;

	Backlight::setOutput();
	Backlight::set();

	display.initialize();
	display.setFont(xpcc::font::Assertion);

	bool dir = true;
	uint8_t y = 0;
	while (1)
	{
		display.clear();
		display.setCursor(46, 2);
		display << "Hello";
		display.setCursor(46, 16);
		display << "World!";

		display.drawLine(0, y, 40, 31 - y);
		display.drawLine(132 - 40, 31 - y, 132, y);
		display.update();

		if (dir) {
			y++;
			if (y >= 31) {
				dir = false;
			}
		}
		else {
			y--;
			if (y == 0) {
				dir = true;
			}
		}
	}
}
