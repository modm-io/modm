
#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>

using namespace xpcc::atmega;

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputC7 Backlight;
	typedef GpioOutputC6 E;
	typedef GpioOutputC5 Rw;
	typedef GpioOutputC4 Rs;
	// note: an 8bit data port
	typedef GpioPort<GpioOutputC0, 8> Data8Bit;
	// and a 4 bit data port
	typedef GpioPort<GpioOutputC4, 4> Data4Bit;
}

// You can choose either port width simply by using it.
// The driver will handle it internally.

// create a LCD object with an 8bit data port
xpcc::Hd44780< lcd::Data8Bit, lcd::Rw, lcd::Rs, lcd::E > display(20, 4);

// create a LCD object with an 4bit data port
//xpcc::Hd44780< lcd::Data4Bit, lcd::Rw, lcd::Rs, lcd::E > display(20, 4);

int
main()
{
	lcd::Backlight::setOutput();
	lcd::Backlight::reset();

	// The LCD needs at least 50ms after power-up until it can be
	// initialized. To make sure this is met we wait here
	xpcc::delayMilliseconds(50);

	display.initialize();
	display.setCursor(0, 0);

	// write the standard welcome message ;-)
	display << "Hello World!\n";

	uint8_t counter = 0;
	while (1)
	{
		// Go to the beginning of the second line of the display and
		// write the value of 'counter'
		display.setCursor(0, 1);
		display << counter << "   ";

		counter++;

		xpcc::delayMilliseconds(200);
	}
}
