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

using namespace Board;

// maps arbitrary gpios to a bit
using LedRingLeft = SoftwareGpioPort<
//	Board::LedSouth,		// 4
	Board::LedSouthWest,	// 3
	Board::LedWest,			// 2
	Board::LedNorthWest,	// 1
	Board::LedNorth			// 0
	>;
// Symmetry \o/
using LedRingRight = SoftwareGpioPort<
//	Board::LedSouth,		// 4
	Board::LedSouthEast,	// 3
	Board::LedEast,			// 2
	Board::LedNorthEast,	// 1
	Board::LedNorth			// 0
	>;

// create the data object
Board::l3g::Gyroscope::Data data;
// and hand it to the sensor driver
Board::l3g::Gyroscope gyro(data);


modm_faststack modm::Fiber<> fiber_gyro([]()
{
	// initialize with limited range of 250 degrees per second
	gyro.configure(gyro.Scale::Dps250);

	modm::filter::MovingAverage<float, 25> averageZ;
	while (true)
	{
		// read out the sensor
		gyro.readRotation();

		// update the moving average
		averageZ.update(gyro.getData().getZ());

		float value = averageZ.getValue();
		// normalize rotation and scale by 5 leds
		uint16_t leds = abs(value / 200 * 5);
		leds = (1ul << leds) - 1;

		// use left or right half ring depending on sign
		if (value < 0) {
			LedRingRight::write(0);
			LedRingLeft::write(leds);
		}
		else {
			LedRingLeft::write(0);
			LedRingRight::write(leds);
		}

		// repeat every 5 ms
		modm::this_fiber::sleep_for(5ms);
	}
});

modm_faststack modm::Fiber<> fiber_blinky([]()
{
	while (true)
	{
		Board::LedSouth::toggle();
		modm::this_fiber::sleep_for(1s);
	}
});

int
main()
{
	Board::initialize();
	Board::initializeL3g();

	modm::fiber::Scheduler::run();

	return 0;
}
