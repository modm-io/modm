
#include <xpcc/architecture.hpp>

#include <xpcc/driver/display.hpp>
#include <xpcc/ui/display.hpp>
#include <xpcc/processing.hpp>

#include "images/rca_logo_128x64.hpp"

using namespace xpcc::atmega;

namespace led
{
	typedef GpioOutputD7 R;
	typedef GpioOutputD6 G;
	typedef GpioOutputD5 B;
}

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputB7 Sck;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 A0;
	typedef GpioOutputD4 Reset;

	typedef xpcc::SoftwareSpiMaster< Sck, Mosi > SPI;
}

xpcc::DogM128< lcd::SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

using namespace xpcc::glcd;

// timer interrupt routine
XPCC_ISR(TIMER2_COMPA)
{
	xpcc::Clock::increment();
}

void
setup()
{
	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;

	// enable interrupts
	sei();

	display.initialize();
}

void
introScreen()
{
	display.drawImage(
			Point(0, 0),
			xpcc::accessor::asFlash(bitmap::rca_logo_128x64));
	display.update();

	xpcc::delayMilliseconds(2000);

	display.clear();

	display.setFont(xpcc::font::ScriptoNarrow);
	display.setCursor(Point(0, 0));
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"§$%&" << xpcc::endl;
	display << "abcdefghijklmnopqrstuvwxyz012345";

	display.setFont(xpcc::font::AllCaps3x5);
	display.setCursor(Point(0, 20));
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"§$%&/()" << xpcc::endl;
	display << "abcdefghijklmnopqrstuvwxyz01234567";

	display.setFont(xpcc::font::FixedWidth5x8);
	display.setCursor(Point(0, 35));
	display << "ABCDEFGHIJKLMNOPQRSTU" << xpcc::endl;
	display << "abcdefghijklmnopqrstu";

	display.update();

	xpcc::delayMilliseconds(1000);
	display.setCursor(Point(40, 55));

	display << "5 ";
	display.update();
	xpcc::delayMilliseconds(1000);

	display << "4 ";
	display.update();
	xpcc::delayMilliseconds(1000);

	display << "3 ";
	display.update();
	xpcc::delayMilliseconds(1000);

	display << "2 ";
	display.update();
	xpcc::delayMilliseconds(1000);

	display << "1";
	display.update();
	xpcc::delayMilliseconds(1000);

	display.clear();
}

// this draws an object that appears to spin
void
drawSpinner(Point center, uint8_t pos)
{
	const uint8_t x = center.getX();
	const uint8_t y = center.getY();

	switch(pos % 8)
	{
		case 0: display.drawLine(x,   y-8, x,   y+8); break;
		case 1: display.drawLine(x+3, y-7, x-3, y+7); break;
		case 2: display.drawLine(x+6, y-6, x-6, y+6); break;
		case 3: display.drawLine(x+7, y-3, x-7, y+3); break;
		case 4: display.drawLine(x+8, y,   x-8, y);	  break;
		case 5: display.drawLine(x+7, y+3, x-7, y-3); break;
		case 6: display.drawLine(x+6, y+6, x-6, y-6); break;
		case 7: display.drawLine(x+3, y+7, x-3, y-7); break;
	}
}


int 
main()
{
	setup();

	introScreen();

	display.setFont(xpcc::font::FixedWidth5x8);

	xpcc::ShortPeriodicTimer timer(1000);
	while (1)
	{
		uint8_t iter = 0;
		while (!timer.execute())
		{
			// rectangle in left side of screen
			display.setColor(Color::black());
			display.drawRectangle(Point(0, 0), 62, 62);

			// rounded rectangle around text area
			display.drawRoundedRectangle(Point(66, 0), 127-66, 62, 5);

			// draw lines from upper left down right side of rectangle
			for(uint8_t i = 0; i < 62; i += 4) {
				display.drawLine(Point(1, 1), Point(63, i));
			}

			// draw circle centered in the left side of screen
			display.drawCircle(Point(31, 31), 31);

			// clear previous spinner position
			display.setColor(Color::white());
			display.fillRectangle(Point(87, 40), 16, 16);

			static uint8_t loops = 0;
			display.setColor(Color::black());
			drawSpinner(Point(95, 48), loops++);

			display.setColor(Color::white());
			display.setCursor(Point(25, 40));
			display << ++iter;
			display.update();
		}

		// print number of iterations in one second
		display.clear();
		display.setCursor(Point(80, 10));
		display.setColor(Color::white());
		display << "FPS=" << iter;
	}
}
