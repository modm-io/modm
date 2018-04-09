/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2014, 2016-2018, Niklas Hauser
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

using namespace modm::platform;
using systemClock = SystemClock;

// Graphic LCD
namespace lcd
{
	typedef GpioOutputB7 Scl;
	typedef GpioInputB6 Miso;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 Rs;
}

typedef BitBangSpiMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

modm::DogM163< SPI, lcd::Cs, lcd::Rs > display;

int
main()
{
	SPI::connect<lcd::Scl::BitBang, lcd::Mosi::BitBang, lcd::Miso::BitBang>();
	SPI::initialize<systemClock, 1000000>();
	lcd::Cs::setOutput();
	lcd::Rs::setOutput();

	display.initialize();

	display.setCursor(0, 0);
	display << "ABCDEFGHIJKLMNOP" << modm::endl;
	display << "abcdefghijklmnop" << modm::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%" << modm::endl;

	while (1)
	{
	}
}
