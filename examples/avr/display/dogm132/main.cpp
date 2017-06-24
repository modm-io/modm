/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2014, 2016-2017, Niklas Hauser
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
#include <modm/driver/display.hpp>
#include <modm/ui/button_group.hpp>

using namespace modm::platform;
using systemClock = SystemClock;

typedef GpioOutputD2 Cs;
typedef GpioOutputB6 Mosi;
typedef GpioOutputB7 Sck;

typedef BitBangSpiMaster<Sck, Mosi> SPI;

typedef GpioOutputD3 A0;
typedef GpioOutputD4 Reset;

typedef modm::DogM132<SPI, Cs, A0, Reset> Display;

typedef GpioOutputD7 Backlight;

int
main()
{
	Display display;

	Backlight::setOutput();
	Backlight::set();

	SPI::connect<Sck::BitBang, Mosi::BitBang>();
	SPI::initialize<systemClock, 1000000>();

	display.initialize();
	display.setFont(modm::font::Assertion);

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
