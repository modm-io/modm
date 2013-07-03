/**
 * Test for LPC2368 processor on
 * Development board
 * "CP-JR ARM7 LPC2368"
 * from www.etteam.com
 *
 * A HD44780 compatible display should be connected to the ET-CLCD connector.
 *
 * Status as of 2012-06-10: works in hardware.
 *
 */

#include <xpcc/architecture.hpp>

#include <xpcc/ui/display.hpp>

// define the pins used by the LCD
namespace lcd {
	GPIO__OUTPUT(E,  1, 31);
	GPIO__OUTPUT(Rw, 1, 29);
	GPIO__OUTPUT(Rs, 1, 28);

	GPIO__IO(D4, 1, 24);
	GPIO__IO(D5, 1, 25);
	GPIO__IO(D6, 1, 26);
	GPIO__IO(D7, 1, 27);

	typedef xpcc::gpio::Nibble<D7, D6, D5, D4> Data;
}

// create an LCD object
xpcc::Hd44780< lcd::E, lcd::Rw, lcd::Rs, lcd::Data > display(20, 4);

int
main(void)
{
	xpcc::lpc::Clock::initialize();

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
		display << counter << "  ";

		counter++;

		xpcc::delay_ms(200);
	}

	return 0;
}
