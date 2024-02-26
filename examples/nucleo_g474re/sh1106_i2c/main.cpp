/*
 * Copyright (c) 2023, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/display/sh1106_i2c.hpp>
// #include <modm/driver/display/ssd1306_i2c.hpp>

/// SH1106 display in I2C mode
using MyI2cMaster = modm::platform::I2cMaster1;
using Scl = Board::D15;
using Sda = Board::D14;

using Display = modm::Sh1106I2c<MyI2cMaster>;
// using Display = modm::Ssd1306<MyI2cMaster>;

Display display;

int
main()
{
	Board::initialize();

	modm::delay(100ms);

	MyI2cMaster::connect<Sda::Sda, Scl::Scl>();
	// Use 20kHz I2C and internal pull-ups, works without external pull-ups
	// if the jumper wires are short enought
	MyI2cMaster::connect<Sda::Sda, Scl::Scl>(MyI2cMaster::PullUps::Internal);

	modm::delay(100ms);

	RF_CALL_BLOCKING(display.initialize());
	RF_CALL_BLOCKING(display.setOrientation(modm::glcd::Orientation::Landscape0));
	RF_CALL_BLOCKING(display.setDisplayMode(Display::DisplayMode::Inverted));
	RF_CALL_BLOCKING(display.setContrast(80));

	display.setFont(modm::font::Assertion);

	modm::ShortPeriodicTimer timer(333ms);
	uint16_t counter(0);

	while (true)
	{
		if (timer.execute())
		{
			display.clear();
			display.setCursor(1, 1);
			display << "Hello World!";
			display.setCursor(1, 17);
			display << counter++;
			display.setCursor(1, 33);
			display << "Line 3";
			display.setCursor(1, 49);
			display << "Line 4";

			display.update();
		}
	}

	return 0;
}
