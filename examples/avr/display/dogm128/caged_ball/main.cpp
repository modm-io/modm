
#include <xpcc/architecture.hpp>

#include <xpcc/driver/display.hpp>

namespace led
{
	typedef GpioOutputD7 R;
	typedef GpioOutputD6 G;
	typedef GpioOutputD5 B;
}

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputB7 Scl;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 A0;
	typedef GpioOutputD4 Reset;

	typedef xpcc::SoftwareSpiMaster< Scl, Mosi, xpcc::GpioUnused > SPI;
}

xpcc::DogM128< lcd::SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

using namespace xpcc::glcd;

int
main()
{
	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	display.initialize();

	int8_t xSpeed = 3;
	int8_t ySpeed = 1;
	uint8_t x = 15;
	uint8_t y = 15;
	const uint8_t radius = 5;
	while (1)
	{
		display.clear();
		display.drawRectangle(Point(10, 10), 104, 44);

		x += xSpeed;
		y += ySpeed;

		if (x < 10 + radius + 3 || x > 114 - radius - 3) {
			xSpeed = -xSpeed;
		}
		if (y < 10 + radius + 1 || y > 54 - radius - 1) {
			ySpeed = -ySpeed;
		}

		display.drawCircle(Point(x, y), radius);
		display.update();

		xpcc::delayMilliseconds(50);
	}
}
