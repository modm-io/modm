/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/platform/platform.hpp>
#include <modm/driver/display/siemens_s65.hpp>
#include <modm/ui/button_group.hpp>

using namespace modm::platform;

using systemClock = SystemClock;

typedef GpioOutputA0 Mosi;
typedef GpioOutputA1 Sck;
typedef BitBangSpiMaster<Sck, Mosi> SPI;

typedef GpioOutputA2 Cs;
typedef GpioOutputA3 Rs;
typedef GpioOutputA4 Reset;
typedef modm::SiemensS65Portrait<SPI, Cs, Rs, Reset> Display;

typedef GpioOutputA5 Backlight;

int
main()
{
	SPI::connect<Sck::BitBang, Mosi::BitBang>();
	SPI::initialize<systemClock, 1000000>();

	Backlight::setOutput();
	Backlight::set();

	Display display;
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
