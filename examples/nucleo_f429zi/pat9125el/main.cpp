/*
 * Copyright (c) 2018 Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>
#include <modm/driver/motion/pat9125el.hpp>

using I2c = I2cMaster1;
using Scl = GpioB8;
using Sda = GpioB9;

// int pin is optional, set to void for polling mode
using Int = GpioInputA5;

class Thread : public modm::pt::Protothread
{
public:
	Thread() : sensor{0x75}
	{
	}

	bool
	update()
	{
		PT_BEGIN();

		MODM_LOG_INFO << "Ping device" << modm::endl;
		// ping the device until it responds
		while(true)
		{
			if (PT_CALL(sensor.ping())) {
				break;
			}
			// otherwise, try again in 100ms
			timeout.restart(100ms);
			PT_WAIT_UNTIL(timeout.isExpired());
		}
		MODM_LOG_INFO << "Ping successful" << modm::endl;

		// set x and y resolution
		PT_CALL(sensor.configure(0x14, 0x14));

		while (true)
		{
			PT_CALL(sensor.readData());
			if(sensor.hasMoved()) {
				position += sensor.getData();

				Board::Leds::write(0b111);
				MODM_LOG_INFO << "Position: " << position.x << ", " << position.y << modm::endl;
				sensor.resetMoved();
			} else {
				Board::Leds::write(0b000);
			}
		}

		PT_END();
	}

private:
	modm::ShortTimeout timeout;
	modm::pat9125el::Motion2D position;
	modm::Pat9125el<modm::Pat9125elI2cTransport<I2c>, Int> sensor;
};

Thread thread;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	Board::Leds::setOutput();

	MODM_LOG_INFO << "\n\nPAT9125EL I2C example\n\n";

	I2c::connect<Sda::Sda, Scl::Scl>();
	I2c::initialize<Board::SystemClock, 400_kHz, 20_pct>();

	while (true) {
		thread.update();
	}

	return 0;
}
