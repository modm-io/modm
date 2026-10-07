/*
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2015-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/math/filter.hpp>

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

		const bool xs = averageX.getValue() < -0.2f;
		const bool xn = averageX.getValue() >  0.2f;

		const bool xe = averageY.getValue() < -0.2f;
		const bool xw = averageY.getValue() >  0.2f;

		Board::LedNorth::set(xn and not (xe or xw));
		Board::LedNorthEast::set(xn and xe);
		Board::LedNorthWest::set(xn and xw);

		Board::LedEast::set(xe and not (xs or xn));
		Board::LedWest::set(xw and not (xs or xn));

		Board::LedSouthEast::set(xs and xe);
		Board::LedSouthWest::set(xs and xw);
		Board::LedSouth::set(xs and not (xe or xw));

		// repeat every 5 ms
		modm::delay(5ms);
	}
	return 0;
}
