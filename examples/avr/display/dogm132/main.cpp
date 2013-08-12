
#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>
#include <xpcc/ui/button_group.hpp>

typedef GpioOutputE4 Cs;
typedef GpioOutputE5 Mosi;
typedef GpioOutputE7 Sck;

typedef xpcc::SoftwareSpiMaster<Sck, Mosi, xpcc::GpioUnused> SPI;

typedef GpioOutputF1 A0;
typedef GpioOutputK3 Reset;

typedef xpcc::DogM132<SPI, Cs, A0, Reset> Display;

GPIO__OUTPUT(Backlight, F, 0);

MAIN_FUNCTION
{
	Display display;

	Backlight::setOutput();
	Backlight::set();

	SPI::initialize();

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
