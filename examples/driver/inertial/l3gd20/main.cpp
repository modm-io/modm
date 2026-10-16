/*
 * Copyright (c) 2014-2018, Niklas Hauser
 * Copyright (c) 2015-2016, Kevin Läufer
 * Copyright (c) 2015, Martin Esser
 * Copyright (c) 2018, Christopher Durand
 * Copyright (c) 2024, Carl Treudler
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/math/filter.hpp>

// create the data object
Board::l3g::Gyroscope::Data data;
// and hand it to the sensor driver
Board::l3g::Gyroscope gyro(data);

modm::filter::MovingAverage<float, 25> averageZ;

int
main()
{
	Board::initialize();
	Board::initializeL3g();

	// initialize with limited range of 250 degrees per second
	gyro.configure(gyro.Scale::Dps250);

	while (true)
	{
		// read out the sensor
		gyro.readRotation();

		// update the moving average
		averageZ.update(gyro.getData().getZ());

		// show the rotation speed as a bar on all LEDs of the board
		constexpr uint16_t width = Board::Leds::width;
		const uint16_t leds = std::min<uint16_t>(std::abs(averageZ.getValue() / 200 * width), width);
		Board::Leds::write((1ul << leds) - 1);

		// repeat every 5 ms
		modm::delay(5ms);
	}

	return 0;
}
