/*
 * Copyright (c) 2011, Fabian Greif
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2014, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture.hpp>
#include <modm/driver/display.hpp>
#include <modm/ui/display/font.hpp>

using namespace xpcc::atmega;
typedef xpcc::avr::SystemClock clock;

// Graphic LCD
namespace lcd
{
	typedef GpioOutputB7 Scl;
	typedef GpioInputB6 Miso;
	typedef GpioOutputB5 Mosi;

	typedef GpioOutputD2 Cs;
	typedef GpioOutputD3 Rs;
}

typedef xpcc::SoftwareSpiMaster< lcd::Scl, lcd::Mosi, lcd::Miso > SPI;

xpcc::DogM163< SPI, lcd::Cs, lcd::Rs > display;

int 
main()
{
    SPI::initialize<clock, 1000000>();
    lcd::Cs::setOutput();
    lcd::Rs::setOutput();

	display.initialize();

	display.setCursor(0, 0);
	display << "ABCDEFGHIJKLMNOP" << xpcc::endl;
	display << "abcdefghijklmnop" << xpcc::endl;
	display << 0 << 12 << 345 << 6789 << "!\"§$%" << xpcc::endl;

	while (1)
	{
	}
}
