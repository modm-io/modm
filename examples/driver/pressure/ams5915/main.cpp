// coding: utf-8
/*
 * Copyright (c) 2017, Raphael Lehmann
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
#include <modm/driver/pressure/ams5915.hpp>

/**
 * Example to demonstrate a MODM driver for pressure sensor AMSYS 5915
 *
 * This example uses I2cMaster2 of STM32F407
 *
 * SDA		PB11
 * SCL		PB10
 *
 * GND and +3V are connected to the sensor.
 */

typedef I2cMaster2 MyI2cMaster;

modm::ams5915::Data data;
modm::Ams5915<MyI2cMaster> pressureSensor(data);

modm::Fiber fiber_sensor([]
{
	MODM_LOG_DEBUG << "Ping the device from ThreadOne" << modm::endl;
	while (not pressureSensor.ping()) modm::this_fiber::sleep_for(100ms);
	MODM_LOG_DEBUG << "Device responded" << modm::endl;

	while (true)
	{
		pressureSensor.readPressure();
		MODM_LOG_INFO << "Pressure [0..1]: " << data.getPressure() << modm::endl;
		MODM_LOG_INFO << "Temperature [degree centigrade]: " << data.getTemperature() << modm::endl;
		modm::this_fiber::sleep_for(1ms);
	}
});

modm::Fiber fiber_blink([]
{
	Board::LedOrange::setOutput();
	while(true)
	{
		Board::LedOrange::toggle();
		modm::this_fiber::sleep_for(0.5s);
	}
});


// ----------------------------------------------------------------------------
int
main()
{
	Board::initialize();

	MyI2cMaster::connect<GpioB10::Scl, GpioB11::Sda>();
	MyI2cMaster::initialize<Board::SystemClock, 400_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to AMSYS 5915 pressure sensor demo!\n\n";

	modm::fiber::Scheduler::run();
	return 0;
}
