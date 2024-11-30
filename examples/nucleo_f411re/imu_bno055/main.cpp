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

modm::Fiber fiber_bno055([]
{
	using namespace modm::this_fiber;
	MODM_LOG_DEBUG << "Ping the device from ThreadOne" << modm::endl;

	// ping the device until it responds
	while(not imu.ping()) sleep_for(100ms);
	MODM_LOG_DEBUG << "Device responded" << modm::endl;

	while(not imu.configure()) sleep_for(100ms);
	MODM_LOG_DEBUG << "Device configured" << modm::endl;

	while (true)
	{
		sleep_for(100ms);
		imu.readData();
		MODM_LOG_INFO << (int)imu.getData().heading() << modm::endl;
	}
});

modm::Fiber fiber_blink([]
{
	LedD13::setOutput();
	while(true)
	{
		LedD13::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});

// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MyI2cMaster::connect<Board::D15::Scl, Board::D14::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to BNO055 demo!\n\n" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}


