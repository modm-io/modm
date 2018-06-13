/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2013, 2017-2018, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform.hpp>
#include <modm/driver/display/hd44780.hpp>

using namespace modm::platform;

// define the pins used by the LCD
namespace lcd
{
	typedef GpioOutputC7 Backlight;
	typedef GpioOutputC6 E;
	typedef GpioOutputC5 Rw;
	typedef GpioOutputC4 Rs;
	// note: an 8bit data port
	typedef GpioPort<GpioOutputC0, 8> Data8Bit;
	// and a 4 bit data port
	typedef GpioPort<GpioOutputC4, 4> Data4Bit;
}

// You can choose either port width simply by using it.
// The driver will handle it internally.

// create a LCD object with an 8bit data port
modm::Hd44780< lcd::Data8Bit, lcd::Rw, lcd::Rs, lcd::E > display(20, 4);

// create a LCD object with an 4bit data port
//modm::Hd44780< lcd::Data4Bit, lcd::Rw, lcd::Rs, lcd::E > display(20, 4);

int
main()
{
	lcd::Backlight::setOutput();
	lcd::Backlight::reset();

	// The LCD needs at least 50ms after power-up until it can be
	// initialized. To make sure this is met we wait here
	modm::delayMilliseconds(50);

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
		display << counter << "   ";

		counter++;

		modm::delayMilliseconds(200);
	}
}
