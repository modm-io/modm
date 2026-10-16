/*
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2015-2018, Niklas Hauser
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

// The LEDs pointing into the four directions of the board
#ifdef STM32F303xC
using LedNorth = Board::LedNorth;
using LedEast = Board::LedEast;
using LedSouth = Board::LedSouth;
using LedWest = Board::LedWest;
#else
using LedNorth = Board::LedOrange;
using LedEast = Board::LedRed;
using LedSouth = Board::LedBlue;
using LedWest = Board::LedGreen;
#endif

// create the data object
Board::lsm3::Accelerometer::Data data;
// and hand it to the sensor driver
Board::lsm3::Accelerometer accelerometer(data);

modm::filter::MovingAverage<float, 25> averageX;
modm::filter::MovingAverage<float, 25> averageY;

int
main()
{
	Board::initialize();
	Board::initializeLsm3();

	// initialize with limited range of ±2g
	accelerometer.configure(accelerometer.Scale::G2);

	while (true)
	{
		// read out the sensor
		accelerometer.readAcceleration();

		averageX.update(accelerometer.getData().getX());
		averageY.update(accelerometer.getData().getY());

		// light up the LEDs on the side the board is tilted towards
		LedSouth::set(averageX.getValue() < -0.2f);
		LedNorth::set(averageX.getValue() >  0.2f);
		LedEast::set(averageY.getValue() < -0.2f);
		LedWest::set(averageY.getValue() >  0.2f);

		// repeat every 5 ms
		modm::delay(5ms);
	}

	return 0;
}
