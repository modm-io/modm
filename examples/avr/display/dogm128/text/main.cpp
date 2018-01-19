/*
 * Copyright (c) 2010-2011, Fabian Greif
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
#include <modm/driver/display/ea_dog.hpp>
#include <modm/ui/display/font.hpp>
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

using systemClock = SystemClock;

// LCD Backlight
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
	typedef GpioInputB6 Miso;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 A0;
	typedef GpioOutputD4 Reset;
}

typedef BitBangSpiMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

modm::DogM128< SPI, lcd::Cs, lcd::A0, lcd::Reset, true > display;

int
main()
{
	// Enable a yellow backlight
	led::R::set();
	led::G::set();
	led::B::reset();

	led::R::setOutput();
	led::G::setOutput();
	led::B::setOutput();

	SPI::connect< lcd::Scl::BitBang, lcd::Mosi::BitBang, lcd::Miso::BitBang >();
	SPI::initialize<systemClock, MHz2>();

	display.initialize();

	display.setFont(modm::font::ScriptoNarrow);
	display.setCursor(modm::glcd::Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";

	display.setFont(modm::font::AllCaps3x5);
	display.setCursor(modm::glcd::Point(0, 32));
	display << "Hello World!" << modm::endl;
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ" << modm::endl;
	display << "abcdefghijklmnopqrstuvwxyz" << modm::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%&/()=?`´,;:-<>";
	display.update();

	modm::delayMilliseconds(2000);

	display.clear();
	display.setFont(modm::font::Assertion);
	display.setCursor(modm::glcd::Point(0, 0));
	display << "Hello World!\n";
	display << "ABCDEFGHIJKLMNOPQRSTUVWXYZ\n";
	display << "abcdefghijklmnopqrstuvwxyz\n";
	display << "0123456789!\"§$%&/()=?`´,;:-<>";
	display.update();

	modm::delayMilliseconds(2000);

	display.clear();
	display.setFont(modm::font::ArcadeClassic);
	display.setCursor(modm::glcd::Point(0, 0));
	display << "Hello World!\n\n";
	display << "ABCDEFGHIJKLMNOP\nQRSTUVWXYZ\n";
	display << "0123456789!\"§$%&/\n()=?`´,;:-<>";
	display.update();

	while (1)
	{
	}
}
