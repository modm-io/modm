
#include <xpcc/architecture.hpp>

#include <xpcc/driver/display.hpp>
#include <xpcc/ui/display/font.hpp>

using namespace xpcc::atmega;

// LCD Backlight
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
	typedef GpioInputB6 Miso;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 A0;
	typedef GpioOutputD4 Reset;
}

typedef xpcc::SoftwareSpiSimpleMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

xpcc::DogM128< SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

MAIN_FUNCTION
{
	// Enable a yellow backlight
	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	display.initialize();

	display.setFont(xpcc::font::ScriptoNarrow);
	display.setCursor(xpcc::glcd::Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";

	display.setFont(xpcc::font::AllCaps3x5);
	display.setCursor(xpcc::glcd::Point(0, 32));
	display << "Hello World!" << xpcc::endl;
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << xpcc::endl;
	display << "abcdefghijklmnopqrstuvwxyz" << xpcc::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%&/()=?`´,;:-<>";
	display.update();

	xpcc::delayMilliseconds(2000);

	display.clear();
	display.setFont(xpcc::font::Assertion);
	display.setCursor(xpcc::glcd::Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";
	display.update();

	xpcc::delayMilliseconds(2000);

	display.clear();
	display.setFont(xpcc::font::ArcadeClassic);
	display.setCursor(xpcc::glcd::Point(0, 0));
	display << "Hello World!\n\n";
	display << "ABCDEFGHIJKLMNOP\nQRSTUVWXYZ\n";
	display << "0123456789!\"§$%&/\n()=?`´,;:-<>";
	display.update();

	while (1)
	{
	}
}
