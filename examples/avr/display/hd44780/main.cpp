
#include <xpcc/architecture.hpp>
#include <xpcc/driver/display.hpp>

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputC7 Backlight;
	typedef GpioOutputC6 E;
	typedef GpioOutputC5 Rw;
	typedef GpioOutputC4 Rs;
	typedef GpioLowNibbleC Data;
}

// create a LCD object
xpcc::Hd44780< lcd::E, lcd::Rw, lcd::Rs, lcd::Data > display(20, 4);

int
main()
{
	lcd::Backlight::setOutput();
	lcd::Backlight::reset();

	// The LCD needs at least 50ms after power-up until it can be
	// initialized. To make sure this is met we wait here
	xpcc::delay_ms(50);

	display.initialize();
	display.setCursor(0, 0);

	// write the standard welcome message ;-)
	display << "Hello World!\n";

	uint8_t counter = 0;
	while (1)
	{
		// Go to the beginning of the second line of the display and
		// write the value of 'counter'
		display.setCursor(1, 0);
		display << counter << "   ";

		counter++;

		xpcc::delay_ms(200);
	}
}
