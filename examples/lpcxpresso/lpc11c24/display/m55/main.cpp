/**
 * \brief	Example with LPC11C24 and Siemens M55 display.
 *
 * The M55 display is connected to the hardware SPI and 3 GPIOs
 *
 *
 */

#include <xpcc/architecture.hpp>
#include <xpcc/communication/spi.hpp>

// How to include the defines?
// #include "xpcc_config.hpp" does not work.
#include "../../../../../build/lpcxpresso/lpc11c24/display_m55/libxpcc/xpcc_config.hpp"
#include <xpcc/ui/display/siemens_m55.hpp>

#include <xpcc/processing/timeout.hpp>

//#define USE_HARDWARE_SPI

namespace lcd
{
	GPIO__OUTPUT(Cs,    2,  2); // Pin 1 at display (to the border)
	GPIO__OUTPUT(Reset, 2,  0); // Pin 2
	GPIO__OUTPUT(Rs,    2,  1);	// Pin 3
}

namespace spi
{
	GPIO__OUTPUT(Sck,  0, 6);	// Pin 4
	GPIO__OUTPUT(Mosi, 0, 9);	// Pin 5
}

#ifdef USE_HARDWARE_SPI
typedef xpcc::lpc::SpiMaster0 SpiDisplay;
#else
typedef xpcc::SoftwareSpiMaster<spi::Sck, spi::Mosi, xpcc::GpioUnused, 2000000> SpiDisplay;
#endif

typedef xpcc::SiemensM55<SpiDisplay, lcd::Cs, lcd::Rs, lcd::Reset> Display;

#define LED_TOGGLE_TICKS 100		// 100 ticks = 1 Hz flash rate
#define COUNT_MAX		3			// how high to count on the LED display

GPIO__OUTPUT(Led, 0, 7);

Display display;

int
main(void)
{
	SystemInit();
	
	xpcc::lpc::SysTickTimer::enable();

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
		static uint16_t ii = 7;
		Led::toggle();
		xpcc::delay_ms(100);
		Led::toggle();
		xpcc::delay_ms(50);

		if (!--ii)
			break;
	}

#ifdef USE_HARDWARE_SPI
	// Initialise the hardware SPI
	SpiDisplay::configurePins(SpiDisplay::MappingSck::PIO0_6);
	SpiDisplay::initialize(SpiDisplay::Mode::MODE_0, SpiDisplay::Presacler::DIV002, 0);
#else
	// Initialise the software SPI
	SpiDisplay::initialize();
#endif

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

		xpcc::delay_ms(1000);
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
