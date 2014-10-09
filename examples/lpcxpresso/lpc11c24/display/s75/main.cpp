/**
 * \brief	Example with LPC11C24 and Siemens S75 display.
 *
 * The S75 display is connected to GPIOs.
 *
 * LPClink does not work.
 * Program with serial boot loader
 * 1) Connect FT/GPIO to GND
 * 2) Power up
 * 3) lpc21isp -debug5 ../../../../../build/lpcxpresso/lpc11c24/display_s75/display_s75.hex  /dev/tty.SLAB_USBtoUART 57600 14746
 * 4) Power down
 * 5) Disconnect FT/GPIO
 * 6) Power up
 *
 * See siemens_s75.hpp for pinout
 *
 */

#include <xpcc/architecture.hpp>

// How to include the defines?
// #include "xpcc_config.hpp" does not work.
#include "../../../../../build/lpcxpresso/lpc11c24/display_s75/libxpcc/xpcc_config.hpp"
#include <xpcc/driver/ui/display/tft_memory_bus.hpp>
#include <xpcc/driver/ui/display/siemens_s75.hpp>

#include <xpcc/processing/timeout.hpp>

namespace lcd
{
	GPIO__IO(D0,   3,  2);
	GPIO__IO(D1,   2,  2);
	GPIO__IO(D2,   2,  1);
	GPIO__IO(D3,   2,  0);
	GPIO__IO(D4,   1,  5);
	GPIO__IO(D5,   2, 10);
	GPIO__IO(D6,   3,  5); // wrong text on silkscreen
	GPIO__IO(D7,   1,  9);

	GPIO__OUTPUT(Cd,    0,  6);		// = CD = Command / Data
	GPIO__OUTPUT(Cs,    3,  1);
	GPIO__OUTPUT(Wr,    3,  4); // wrong text on silkscreen

	GPIO__OUTPUT(Reset, 3,  0);

	typedef xpcc::gpio::Port<D7, D6, D5, D4, D3, D2, D1, D0> Port;
	typedef xpcc::TftMemoryBus8BitGpio<Port, Cs, xpcc::gpio::Unused, Wr, Cd> ParallelBus;

	// Display
	typedef xpcc::SiemensS75LandscapeRight<lcd::ParallelBus, lcd::Reset> Display;

}

#define LED_TOGGLE_TICKS 100		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

GPIO__OUTPUT(Led, 0, 7);

lcd::ParallelBus
parallelBus;

lcd::Display display(parallelBus);

int
main(void)
{
	SystemInit();
	
	xpcc::lpc::SysTickTimer::enable();

	// Set LED port pin to output
	Led::setOutput();
	
	while (1)
	{
		static uint16_t ii = 10;
		Led::toggle();
		xpcc::delayMilliseconds(100);
		Led::toggle();
		xpcc::delayMilliseconds(50);

		if (!--ii)
			break;
	}

	// Initialise the display
	lcd::Reset::setOutput();

	lcd::Cd::setOutput();
	lcd::Cs::setOutput();
	lcd::Wr::setOutput();

	lcd::D0::setInput();
	lcd::D1::setInput();
	lcd::D2::setInput();
	lcd::D3::setInput();
	lcd::D4::setInput();
	lcd::D5::setInput();
	lcd::D6::setInput();
	lcd::D7::setInput();

	lcd::ParallelBus::initialize();

	display.initialize();
	display.setFont(xpcc::font::Assertion);

	bool dir = true;
	uint8_t y = 0;
	while (1)
	{
		display.clear();
		display.setCursor(5, y);
		display << "Hello";
		display.setCursor(46, 16);
		display << "World!";

		display.drawLine(0, y, 40, 31 - y);
		display.drawLine(132 - 40, 31 - y, 132, y);

		if (dir) {
			y++;
			if (y >= 5) {
				dir = false;
			}
		}
		else {
			y--;
			if (y == 0) {
				dir = true;
			}
		}
		// finished, copy to lcd
		display.update();
	}
}
