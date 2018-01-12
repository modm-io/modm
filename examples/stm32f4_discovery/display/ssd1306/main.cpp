/*
 * Copyright (c) 2014-2017, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board/board.hpp>
#include <modm/driver/display/ssd1306.hpp>

/**
 * Example to demonstrate a MODM graphics display SSD1306.
 *
 * PB9	SDA
 * PB8	SCL
 *
 * GND and +5V or +3V3 are connected to the display module.
 */

typedef GpioB9 Sda;
typedef GpioB8 Scl;
typedef I2cMaster1 MyI2cMaster;
modm::Ssd1306<MyI2cMaster> display;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MyI2cMaster::connect<Scl::Scl, Sda::Sda>();
	MyI2cMaster::initialize<Board::systemClock, 420000>();

	display.initializeBlocking();
	display.setFont(modm::font::Assertion);
	display << "Hello World!";
	display.update();

	modm::ShortPeriodicTimer timer(1000);
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
