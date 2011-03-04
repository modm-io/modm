
#include <xpcc/architecture/driver.hpp>

#include <xpcc/driver/lcd/ea_dog.hpp>
#include <xpcc/driver/lcd/font.hpp>
#include <xpcc/driver/software_spi.hpp>
#include <xpcc/workflow.hpp>

#include "images/rca_logo_128x64.hpp"

namespace led
{
	GPIO__OUTPUT(R, D, 7);
	GPIO__OUTPUT(G, D, 6);
	GPIO__OUTPUT(B, D, 5);
}

// define the pins used by the LCD
namespace lcd
{
	GPIO__OUTPUT(Scl, B, 7);
	GPIO__OUTPUT(Mosi, B, 5);
	
	GPIO__OUTPUT(CS, D, 2);
	GPIO__OUTPUT(A0, D, 3);
	GPIO__OUTPUT(Reset, D, 4);
	
	typedef xpcc::SoftwareSpi< Scl, Mosi, xpcc::gpio::Unused, 10000000UL > SPI;
}

xpcc::DogM128< lcd::SPI, lcd::CS, lcd::A0, lcd::Reset, true > display;

using namespace xpcc::glcd;

// timer interrupt routine
ISR(TIMER2_COMPA_vect)
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
	
	xpcc::delay_ms(2000);
	
	display.clear();
	
	display.setFont(xpcc::font::ScriptoNarrow);
	display.setCursor(Point(0, 0));
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"§$%&" << xpcc::endl;
	display << "abcdefghijklmnopqrstuvwxyz012345";
	
	display.setFont(xpcc::font::AllCaps3x6);
	display.setCursor(Point(0, 20));
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"§$%&/()" << xpcc::endl;
	display << "abcdefghijklmnopqrstuvwxyz01234567";
	
	display.setFont(xpcc::font::FixedWidth5x8);
	display.setCursor(Point(0, 35));
	display << "ABCDEFGHIJKLMNOPQRSTU" << xpcc::endl;
	display << "abcdefghijklmnopqrstu";
	
	display.update();
	
	xpcc::delay_ms(1000);
	display.setCursor(Point(40, 55));
	
	display << "5 ";
	display.update();
	xpcc::delay_ms(1000);
	
	display << "4 ";
	display.update();
	xpcc::delay_ms(1000);
	
	display << "3 ";
	display.update();
	xpcc::delay_ms(1000);
	
	display << "2 ";
	display.update();
	xpcc::delay_ms(1000);
	
	display << "1";
	display.update();
	xpcc::delay_ms(1000);
	
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


MAIN_FUNCTION
{
	setup();
	
	introScreen();
	
	display.setFont(xpcc::font::FixedWidth5x8);
	
	xpcc::PeriodicTimer<> timer(1000);
	while (1)
	{
		uint8_t iter = 0;
		while (!timer.isExpired())
		{
			// rectangle in left side of screen
			display.setColor(BLACK);
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
			display.setColor(WHITE);
			display.fillRectangle(Point(87, 40), 16, 16);
			
			static uint8_t loops = 0;
			display.setColor(BLACK);
			drawSpinner(Point(95, 48), loops++);
			
			display.setColor(WHITE);
			display.setCursor(Point(25, 40));
			display << ++iter;
			display.update();
		}
		
		// print number of iterations in one second
		display.clear();
		display.setCursor(Point(80, 10));
		display.setColor(WHITE);
		display << "FPS=" << iter;
	}
}
