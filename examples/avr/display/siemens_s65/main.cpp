/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2016, Niklas Hauser
 * Copyright (c) 2012, 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/architecture.hpp>
#include <modm/driver/display.hpp>
#include <modm/ui/button_group.hpp>

using namespace xpcc::atmega;

typedef GpioOutputA0 Mosi;
typedef GpioOutputA1 Sck;
typedef xpcc::SoftwareSpiMaster<Sck, Mosi> SPI;

typedef GpioOutputA2 Cs;
typedef GpioOutputA3 Rs;
typedef GpioOutputA4 Reset;
typedef xpcc::SiemensS65Portrait<SPI, Cs, Rs, Reset> Display;

typedef GpioOutputA5 Backlight;

int 
main()
{
	Display display;

	Backlight::setOutput();
	Backlight::set();

	display.initialize();
	display.setFont(xpcc::font::Assertion);

	bool dir = true;
	uint8_t y = 0;
	while (1)
	{
		display.clear();
		display.setCursor(46, 2);
		display << "Hello";
		display.setCursor(46, 16);
		display << "World!";

		display.drawLine(0, y, 40, 31 - y);
		display.drawLine(132 - 40, 31 - y, 132, y);
		display.update();

		if (dir) {
			y++;
			if (y >= 31) {
				dir = false;
			}
		}
		else {
			y--;
			if (y == 0) {
				dir = true;
			}
		}
	}
}
