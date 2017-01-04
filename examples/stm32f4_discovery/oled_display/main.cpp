/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/platform.hpp>
#include <modm/driver/display/ssd1306.hpp>

/**
 * Example to demonstrate a XPCC graphics display SSD1306.
 *
 * PB9	SDA
 * PB8	SCL
 *
 * GND and +5V or +3V3 are connected to the display module.
 */

typedef GpioB9 Sda;
typedef GpioB8 Scl;
typedef I2cMaster1 MyI2cMaster;
xpcc::Ssd1306<MyI2cMaster> display;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	Sda::connect(MyI2cMaster::Sda);
	Scl::connect(MyI2cMaster::Scl);
	MyI2cMaster::initialize<Board::systemClock, 420000>();

	display.initializeBlocking();
	display.setFont(xpcc::font::Assertion);
	display << "Hello World!";
	display.update();

	xpcc::ShortPeriodicTimer timer(1000);
	uint16_t counter(0);

	while (1)
	{
		if (timer.execute())
		{
			display.setCursor(0,20);
			display << counter++;
			display.update();
		}
	}

	return 0;
}
