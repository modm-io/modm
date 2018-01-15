/*
 * Copyright (c) 2015, Kevin Läufer
 * Copyright (c) 2015-2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board/board.hpp>
#include <modm/processing/processing.hpp>
#include <modm/math/filter.hpp>

using namespace Board;

// create the data object
Board::lsm3::Accelerometer::Data data;
// and hand it to the sensor driver
Board::lsm3::Accelerometer accelerometer(data);


class ReaderThread : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		// initialize with limited range of ±2g
		PT_CALL(accelerometer.configure(accelerometer.Scale::G2));

		while (true)
		{
			// read out the sensor
			PT_CALL(accelerometer.readAcceleration());

			averageX.update(accelerometer.getData().getX());
			averageY.update(accelerometer.getData().getY());

			{
				bool xs = averageX.getValue() < -0.2;
				bool xn = averageX.getValue() >  0.2;

				bool xe = averageY.getValue() < -0.2;
				bool xw = averageY.getValue() >  0.2;

				Board::LedNorth::set(xn and not (xe or xw));
				Board::LedNorthEast::set(xn and xe);
				Board::LedNorthWest::set(xn and xw);

				Board::LedEast::set(xe and not (xs or xn));
				Board::LedWest::set(xw and not (xs or xn));

				Board::LedSouthEast::set(xs and xe);
				Board::LedSouthWest::set(xs and xw);
				Board::LedSouth::set(xs and not (xe or xw));
			}

			// repeat every 5 ms
			this->timeout.restart(5);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::filter::MovingAverage<float, 25> averageX;
	modm::filter::MovingAverage<float, 25> averageY;
};

ReaderThread reader;


int
main()
{
	Board::initialize();
	Board::initializeLsm3();

	while (1)
	{
		reader.update();
	}

	return 0;
}
