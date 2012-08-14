/**
 * \brief	Example with LPC11C24 and Siemens S65 display.
 *
 * The S65 display is connected to GPIOs. Pinout is as follows:
 *
 *
 */

#include <xpcc/architecture.hpp>
#include <xpcc/driver/connectivity/spi.hpp>

// How to include the defines?
// #include "xpcc_config.hpp" does not work.
#include "../../../../../build/lpcxpresso/lpc11c24/display_s65/libxpcc/xpcc_config.hpp"
#include <xpcc/driver/ui/display/siemens_s65.hpp>

#include <xpcc/workflow/timeout.hpp>

#define USE_HARDWARE_SPI

namespace lcd
{
	GPIO__OUTPUT(Rs,    1,  7);
	GPIO__OUTPUT(Reset, 2,  0);
	GPIO__OUTPUT(Cs,    2, 11);
//	GPIO__OUTPUT(Backlight, F, 0);
}

namespace spi
{
	GPIO__OUTPUT(Sck,  0, 6);
	GPIO__OUTPUT(Mosi, 0, 9);
}

#ifdef USE_HARDWARE_SPI
typedef xpcc::lpc::SpiMaster0 SpiDisplay;
#else
typedef xpcc::SoftwareSpi<spi::Sck, spi::Mosi, xpcc::gpio::Unused, 2000000> SpiDisplay;
#endif

typedef xpcc::SiemensS65<SpiDisplay, lcd::Cs, lcd::Rs, lcd::Reset> Display;

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
		static uint16_t ii = 500;
		Led::toggle();
		xpcc::delay_us(100);
		Led::toggle();
		xpcc::delay_us(50);

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

		if (1==2)
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
		} // show 1

		if (1==1)
		{
			display.clear();

			// text demo
			display.setFont(xpcc::font::Assertion);
			display.setCursor(xpcc::glcd::Point(0, 0));
//			display << "ABCDEFGHIJKLMNOP\nQRSTUVWXYZ\n";
//			display << "0123456789!\"§$%&/\n()=?`´,;:-<>\n";

			// FPS display
			static uint8_t fps = 0;
			static uint32_t ii = 0;

			if (fpsTimeout.isExpired())
			{
				fpsTimeout.restart(1000);
				static uint32_t ii_old = 0;
				Led::toggle();
				fps = ii - ii_old;
				ii_old = ii;
			}
			display.printf("fps = %2d\n", fps);

			display.setFont(xpcc::font::Numbers14x32);
			display.printf("%06d", ii++);
			if (ii >= 999999) {
				ii = 0;
			}




			// bouncing ball
			static int8_t xSpeed = 3;
			static int8_t ySpeed = 1;
			static uint8_t x = 15;
			static uint8_t y = 90;
			const uint8_t radius = 5;

			display.drawRectangle(xpcc::glcd::Point(10, 78),
					display.getWidth()-20,
					display.getHeight()-88);

			x += xSpeed;
			y += ySpeed;

			if (x < 10 + radius + 3 || x > display.getWidth() - radius - 13) {
				xSpeed = -xSpeed;
			}
			if (y < 78 + radius + 1 || y > display.getHeight() - 10 - radius - 1) {
				ySpeed = -ySpeed;
			}

			display.drawCircle(xpcc::glcd::Point(x, y), radius);

			// finished, copy to lcd
			display.update();
		}
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
