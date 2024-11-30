// coding: utf-8
/*
 * Copyright (c) 2017, Christopher Durand
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/board.hpp>

#include <modm/architecture/interface/clock.hpp>
#include <modm/processing.hpp>
#include <modm/driver/position/vl53l0.hpp>
#include <modm/debug/logger.hpp>

// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::DEBUG

/**
 * Example to demonstrate a MODM driver for distance sensor VL53L0X
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

modm::vl53l0::Data data;
modm::Vl53l0<MyI2cMaster> distance(data);

// in microseconds
// useful value range: from 20000us = 20ms to 2000000us = 2s
constexpr uint32_t MeasurementTimeFast      = 30000;
constexpr uint32_t MeasurementTimePrecision = 200000;

modm::Fiber fiber_name([]
{
	using namespace modm::this_fiber;
	MODM_LOG_DEBUG << "Ping the device from ThreadOne" << modm::endl;

	// ping the device until it responds

	while(not distance.ping()) sleep_for(100ms);
	MODM_LOG_DEBUG << "Device responded" << modm::endl;

	while(not distance.initialize()) sleep_for(100ms);
	MODM_LOG_DEBUG << "Device initialized" << modm::endl;

	// set measurement time to 200ms (high accuracy mode)
	bool highAccuracyMode = distance.setMaxMeasurementTime(MeasurementTimePrecision);
	if(not highAccuracyMode) {
		MODM_LOG_DEBUG << "Setting measurement time failed" << modm::endl;
	}

	modm::ShortPeriodicTimer timer(1s);

	while (true)
	{
		modm::Timestamp stamp = modm::Clock::now();

		if (distance.readDistance())
		{
			modm::vl53l0::RangeErrorCode error = distance.getRangeError();
			if (distance.getData().isValid())
			{
				uint16_t mm = distance.getData().getDistance();
				MODM_LOG_DEBUG << "mm: " << mm;
			}
			else
			{
				MODM_LOG_DEBUG << "Error: " << static_cast<uint8_t>(error);
			}
		}
		MODM_LOG_DEBUG << ", t = " << (modm::Clock::now() - stamp) << "ms" << modm::endl;

		// query button state every 1s
		if(timer.execute())
		{
			// toggle between fast and high accuracy mode when button is pressed
			if(Button::read())
			{
				if(highAccuracyMode)
				{
					if(distance.setMaxMeasurementTime(MeasurementTimeFast)) {
						MODM_LOG_DEBUG << "Enable fast mode" << modm::endl;
						highAccuracyMode = false;
					} else {
						MODM_LOG_DEBUG << "Setting measurement time failed" << modm::endl;
					}
				}
				else
				{
					if(distance.setMaxMeasurementTime(MeasurementTimePrecision)) {
						MODM_LOG_DEBUG << "Enable high accuracy mode" << modm::endl;
						highAccuracyMode = true;
					} else {
						MODM_LOG_DEBUG << "Setting measurement time failed" << modm::endl;
					}
				}
			}
		}

		sleep_for(100ms);
	}
});

modm::Fiber fiber_blink([]
{
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
	LedD13::setOutput();

	MyI2cMaster::connect<GpioB8::Scl, GpioB9::Sda>();
	MyI2cMaster::initialize<SystemClock, 400_kHz>();

	MODM_LOG_INFO << "\n\nWelcome to VL53L0X demo!\n\n";

	modm::fiber::Scheduler::run();

	return 0;
}
