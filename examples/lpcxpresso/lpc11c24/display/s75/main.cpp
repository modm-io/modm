/**
 * \brief	Example with LPC11C24 and Siemens S75 display.
 *
 * The S75 display is connected to GPIOs. Pinout is as follows:
 *
 *
 */

#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/spi.hpp>

// How to include the defines?
// #include "xpcc_config.hpp" does not work.
#include "../../../../../build/lpcxpresso/lpc11c24/display_s75/libxpcc/xpcc_config.hpp"
#include <xpcc/driver/ui/display/siemens_s75.hpp>

#include <xpcc/workflow/timeout.hpp>

namespace lcd
{
	GPIO__OUTPUT(D0,   3,  2);
	GPIO__OUTPUT(D1,   2,  2);
	GPIO__OUTPUT(D2,   2,  1);
	GPIO__OUTPUT(D3,   2,  0);
	GPIO__OUTPUT(D4,   1,  5);
	GPIO__OUTPUT(D5,   2, 11);
	GPIO__OUTPUT(D6,   0,  8);
	GPIO__OUTPUT(D7,   0,  9);

	typedef xpcc::gpio::Port<D7, D6, D5, D4, D3, D2, D1, D0> Port;

	GPIO__OUTPUT(Rs,    0,  6);		// = CD = Command / Data
	GPIO__OUTPUT(Reset, 3,  0);
	GPIO__OUTPUT(Cs,    3,  1);
	GPIO__OUTPUT(Wr,    1,  4);
}

typedef xpcc::SiemensS75Portrait<lcd::Port, lcd::Cs, lcd::Rs, lcd::Wr, lcd::Reset> Display;

#define LED_TOGGLE_TICKS 100		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

GPIO__OUTPUT(Led, 0, 7);

Display display;

int
main(void)
{
	SystemInit();
	
	xpcc::lpc11::SysTickTimer::enable();

	// Initialize 32-bit timer 0. TIME_INTERVAL is defined as 10mS
	// You may also want to use the Cortex SysTick timer to do this
	init_timer32(0, TIME_INTERVAL);
	
	// Enable timer 0. Our interrupt handler will begin incrementing
	// the TimeTick global each time timer 0 matches and resets.
	enable_timer32(0);
	
	// Set LED port pin to output
	Led::setOutput();
	
	while (1)
	{
		static uint16_t ii = 10;
		Led::toggle();
		xpcc::delay_ms(100);
		Led::toggle();
		xpcc::delay_ms(50);

		if (!--ii)
			break;
	}

	lcd::Port::setOutput();

	// Initialise the display
	display.initialize();
	display.setFont(xpcc::font::Assertion);

	xpcc::Timeout<> fpsTimeout(1000);

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


	while (1)
	{
		// Each time we wake up...
		// Check TimeTick to see whether to set or clear the LED I/O pin
		if ((timer32_0_counter % (LED_TOGGLE_TICKS / COUNT_MAX)) < ((LED_TOGGLE_TICKS / COUNT_MAX) / 2)) {
			Led::reset();
		}
		else {
			Led::set();
		}
		
		// Go to sleep to save power between timer interrupts
		__WFI();
	}
}
