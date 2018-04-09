/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2018, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/architecture/interface/interrupt.hpp>
#include <modm/driver/display/ea_dog.hpp>
#include <modm/processing/timer.hpp>

#include "images/rca_logo_128x64.hpp"

using namespace modm::platform;

using systemClock = SystemClock;

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

	typedef BitBangSpiMaster< Sck, Mosi > SPI;
}

modm::DogM128< lcd::SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

using namespace modm::glcd;

// timer interrupt routine
MODM_ISR(TIMER2_COMPA)
{
	modm::Clock::increment();
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

	lcd::SPI::connect<lcd::Sck::BitBang, lcd::Mosi::BitBang>();
	lcd::SPI::initialize<systemClock, MHz2>();

	// timer initialization
	// compare-match-interrupt every 1 ms at 14.7456 MHz
	TCCR2A = (1 << WGM21);
	TCCR2B = (1 << CS22);
	TIMSK2 = (1 << OCIE2A);
	OCR2A = 230;

	// enable interrupts
	enableInterrupts();

	display.initialize();
}

void
introScreen()
{
	display.drawImage(
			Point(0, 0),
			modm::accessor::asFlash(bitmap::rca_logo_128x64));
	display.update();

	modm::delayMilliseconds(2000);

	display.clear();

	display.setFont(modm::font::ScriptoNarrow);
	display.setCursor(Point(0, 0));
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"§$%&" << modm::endl;
	display << "abcdefghijklmnopqrstuvwxyz012345";

	display.setFont(modm::font::AllCaps3x5);
	display.setCursor(Point(0, 20));
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ!\"§$%&/()" << modm::endl;
	display << "abcdefghijklmnopqrstuvwxyz01234567";

	display.setFont(modm::font::FixedWidth5x8);
	display.setCursor(Point(0, 35));
	display << "ABCDEFGHIJKLMNOPQRSTU" << modm::endl;
	display << "abcdefghijklmnopqrstu";

	display.update();

	modm::delayMilliseconds(1000);
	display.setCursor(Point(40, 55));

	display << "5 ";
	display.update();
	modm::delayMilliseconds(1000);

	display << "4 ";
	display.update();
	modm::delayMilliseconds(1000);

	display << "3 ";
	display.update();
	modm::delayMilliseconds(1000);

	display << "2 ";
	display.update();
	modm::delayMilliseconds(1000);

	display << "1";
	display.update();
	modm::delayMilliseconds(1000);

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

	display.setFont(modm::font::FixedWidth5x8);

	modm::ShortPeriodicTimer timer(1000);
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
