/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2015, 2017-2018, Niklas Hauser
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
#include <modm/driver/display/ea_dog.hpp>
#include <modm/architecture/interface/clock.hpp>

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

	display.drawLine(Point(68, 50), Point(90, 50));
	display.drawLine(Point(78, 45), Point(78, 58));

	display.drawRoundedRectangle(Point(70, 13), 30, 20, 3);

	display.drawCircle(Point(105, 25), 20);

	display.drawEllipse(Point(60, 25), 20, 18);
	display.drawEllipse(Point(63, 31), 63, 31);

	display.fillRectangle(Point(10, 10), 20, 20);
	display.fillCircle(Point(45, 40), 23);
	display.setColor(Color::white());
	display.fillRectangle(Point(20, 20), 20, 20);
	display.fillCircle(Point(55, 50), 8);

	display.setColor(Color::black());
	display.drawLine(Point(0, 0), Point(127, 63));

	display.update();

	while (1)
	{
	}
}
