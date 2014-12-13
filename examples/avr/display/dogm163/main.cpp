
#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>
#include <xpcc/ui/display/font.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// Graphic LCD
namespace lcd
{
	typedef GpioOutputB7 Scl;
	typedef GpioInputB6 Miso;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 Rs;
}

typedef xpcc::SoftwareSpiMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

xpcc::DogM163< SPI, lcd::Cs, lcd::Rs > display;

MAIN_FUNCTION
{
    SPI::initialize<clock, 1000000>();
    lcd::Cs::setOutput();
    lcd::Rs::setOutput();

	display.initialize();

	display.setCursor(0, 0);
	display << "ABCDEFGHIJKLMNOP" << xpcc::endl;
	display << "abcdefghijklmnop" << xpcc::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%" << xpcc::endl;

	while (1)
	{
	}
}
