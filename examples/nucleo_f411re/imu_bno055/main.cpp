// coding: utf-8
/*
 * Copyright (c) 2019, Niklas Hauser
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
#include <modm/driver/inertial/bno055.hpp>
#include <modm/debug.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

/**
 * Example to demonstrate a MODM driver for imu sensor VL53L0X
 *
 * This example uses I2cMaster1 of STM32F401
 *
 * SDA		PB9
 * SCL		PB8
 *
 * GND and +3V are connected to the sensor.
 */

using namespace Board;

using MyI2cMaster = I2cMaster1;
// using MyI2cMaster = BitBangI2cMaster<Board::D15, Board::D14>;

modm::bno055::Data data;
modm::Bno055<MyI2cMaster> imu(data);

class ThreadOne : public modm::pt::Protothread
{
public:
	bool
	update()
	{
		PT_BEGIN();

		MODM_LOG_DEBUG << "Ping the device from ThreadOne" << modm::endl;

		// ping the device until it responds
		while (true)
		{
			// we wait until the device started
			if (PT_CALL(imu.ping())) {
				break;
			}
			PT_WAIT_UNTIL(timer.execute());
		}

		MODM_LOG_DEBUG << "Device responded" << modm::endl;

		while (true)
		{
			if (PT_CALL(imu.configure())) {
				break;
			}

			PT_WAIT_UNTIL(timer.execute());
		}

		MODM_LOG_DEBUG << "Device configured" << modm::endl;

		while (true)
		{
			PT_WAIT_UNTIL(timer.execute());
			PT_CALL(imu.readData());
			MODM_LOG_INFO << (int)imu.getData().heading() << modm::endl;
		}

		PT_END();
	}

private:
	modm::ShortPeriodicTimer timer{100ms};
};

ThreadOne one;

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();
	LedD13::setOutput();

	// Board::D13::setOutput(modm::Gpio::Low);
	MyI2cMaster::connect<Board::D15::Scl, Board::D14::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to BNO055 demo!\n\n" << modm::endl;

	modm::ShortPeriodicTimer tmr(500ms);

	// Board::D15::setOutput();

	while (true)
	{
		one.update();
		if(tmr.execute()) {
			LedD13::toggle();
			// Board::D15::toggle();
		}

	}

	return 0;
}
