
#include <xpcc/architecture.hpp>
#include <xpcc/ui/display.hpp>

#include "hd44780_minimal.hpp"

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputC6 E;
	typedef GpioOutputC5 Rw;
	typedef GpioOutputC4 Rs;

	typedef xpcc::SoftwareGpioNibble<GpioD2, GpioC7, GpioB1, GpioA4> Data;
	//typedef GpioLowNibbleC Data;
}

// create a LCD object
xpcc::Hd44780Minimal< lcd::E, lcd::Rw, lcd::Rs, lcd::Data > display(20, 4);

int
main()
{
	// The LCD needs at least 50ms after power-up until it can be
	// initialized.
	xpcc::delay_ms(50);

	display.initialize();
	display.setCursor(0, 0);

	// write the standard welcome message ;-)
	display.write("Hello World!\n");

	while (1)
	{
	}
}
