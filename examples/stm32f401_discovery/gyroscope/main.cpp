/*
 * Copyright (c) 2014-2018, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
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
#include <modm/driver/inertial/l3gd20.hpp>
#include <modm/processing.hpp>
#include <modm/math/filter.hpp>

// maps arbitrary gpios to a bit
using LedRing = SoftwareGpioPort<
	Board::LedOrange,	// 3
	Board::LedRed,		// 2
	Board::LedBlue,		// 1
	Board::LedGreen		// 0
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
			timeout.restart(5ms);
			PT_WAIT_UNTIL(timeout.isExpired());
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

	while (true)
	{
		reader.update();
	}

	return 0;
}
