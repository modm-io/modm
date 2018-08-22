/*
 * Copyright (c) 2015-2016, Kevin Läufer
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
#include <modm/processing/protothread.hpp>
#include <modm/processing/timer.hpp>
#include <modm/math/filter.hpp>

using namespace Board;

// maps arbitrary gpios to a bit
using LedRing = SoftwareGpioPort<
	Board::LedUp,		// 3
	Board::LedRight,	// 2
	Board::LedDown,		// 1
	Board::LedLeft		// 0
	>;

// create the data object
Board::l3g::Gyroscope::Data data;
// and hand it to the sensor driver
Board::l3g::Gyroscope gyro(data);


class ReaderThread : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		// initialize with limited range of 250 degrees per second
		PT_CALL(gyro.configure(gyro.Scale::Dps250));

		while (true)
		{
			// read out the sensor
			PT_CALL(gyro.readRotation());

			// update the moving average
			averageZ.update(gyro.getData().getZ());

			{
				float value = averageZ.getValue();
				// normalize rotation and scale by 5 leds
				uint16_t leds = abs(value / 200 * 5);
				leds = (1ul << leds) - 1;

				LedRing::write(leds);
			}

			// repeat every 5 ms
			this->timeout.restart(5);
			PT_WAIT_UNTIL(this->timeout.isExpired());
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::filter::MovingAverage<float, 25> averageZ;
};

ReaderThread reader;


int
main()
{
	Board::initialize();
	Board::initializeL3g();

	while (1)
	{
		reader.update();
	}

	return 0;
}
