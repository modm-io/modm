
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

	display.drawLine(Point(68, 50), Point(90, 50));
	display.drawLine(Point(78, 45), Point(78, 58));

	display.drawRoundedRectangle(Point(70, 13), 30, 20, 3);

	display.drawCircle(Point(105, 25), 20);

	display.drawEllipse(Point(60, 25), 20, 18);
	display.drawEllipse(Point(63, 31), 63, 31);

	display.fillRectangle(Point(10, 10), 20, 20);
	display.fillCircle(Point(45, 40), 23);

	display.setColor(WHITE);
	display.fillRectangle(Point(20, 20), 20, 20);
	display.fillCircle(Point(55, 50), 8);

	display.setColor(BLACK);
	display.drawLine(Point(0, 0), Point(127, 63));

	display.update();

	while (1)
	{
	}
}
