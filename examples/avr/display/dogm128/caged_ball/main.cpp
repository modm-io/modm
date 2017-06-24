/*
 * Copyright (c) 2010-2011, Fabian Greif
 * Copyright (c) 2012-2015, 2017, Niklas Hauser
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
	typedef GpioOutputB7 Scl;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 A0;
	typedef GpioOutputD4 Reset;

	typedef BitBangSpiMaster< Scl, Mosi > SPI;
}

modm::DogM128< lcd::SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

using namespace modm::glcd;

int
main()
{
	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	lcd::SPI::connect<lcd::Scl::BitBang, lcd::Mosi::BitBang>();
	lcd::SPI::initialize<systemClock, MHz2>();

	display.initialize();

	int8_t xSpeed = 3;
	int8_t ySpeed = 1;
	uint8_t x = 15;
	uint8_t y = 15;
	const uint8_t radius = 5;
	while (1)
	{
		display.clear();
		display.drawRectangle(Point(10, 10), 104, 44);

		x += xSpeed;
		y += ySpeed;

		if (x < 10 + radius + 3 || x > 114 - radius - 3) {
			xSpeed = -xSpeed;
		}
		if (y < 10 + radius + 1 || y > 54 - radius - 1) {
			ySpeed = -ySpeed;
		}

		display.drawCircle(Point(x, y), radius);
		display.update();

		modm::delayMilliseconds(50);
	}
}
