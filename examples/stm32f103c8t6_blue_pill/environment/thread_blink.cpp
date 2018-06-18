/*
 * Copyright (c) 2016, 2018, Sascha Schade
 * Copyright (c) 2017, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <inttypes.h>
#include <modm/board.hpp>
#include <modm/debug/logger.hpp>

#include "threads.hpp"

// ----------------------------------------------------------------------------
// Set the log level
#undef	MODM_LOG_LEVEL
#define	MODM_LOG_LEVEL modm::log::INFO

// ----------------------------------------------------------------------------
BlinkThread::BlinkThread() :
	timer(5000),
	uptime(0)
{
}

bool
BlinkThread::update()
{
	PT_BEGIN();

	while (true)
	{
		PT_WAIT_UNTIL(timer.execute());
		timeout.restart(100);
		Board::LedGreen::set();

		bmp180Thread.startMeasurement();
		bme280Thread.startMeasurement();


		PT_WAIT_UNTIL(timeout.isExpired()) ;
		Board::LedGreen::reset();

		PT_WAIT_UNTIL(bmp180Thread.isNewDataAvailable() and bme280Thread.isNewDataAvailable());

		int16_t bmp180_a_temp  = bmp180Thread.getTemperatureA();
		int32_t bmp180_a_press = bmp180Thread.getPressureA();
		int32_t bme280_a_temp  = bme280Thread.getTemperatureA();
		int32_t bme280_a_press = bme280Thread.getPressureA();
		int32_t bme280_a_humid = bme280Thread.getHumidityA();

		int16_t bmp180_b_temp  = bmp180Thread.getTemperatureB();
		int32_t bmp180_b_press = bmp180Thread.getPressureB();
		int32_t bme280_b_temp  = bme280Thread.getTemperatureB();
		int32_t bme280_b_press = bme280Thread.getPressureB();
		int32_t bme280_b_humid = bme280Thread.getHumidityB();

		MODM_LOG_INFO.printf("%" PRId32 ",%d,%" PRId32 ",%d,%" PRId32 ",%" PRId32 ",%" PRId32 ",%" PRId32 ",%" PRId32 ",% " PRId32 " ,%" PRId32 "\n",
			uptime,
			bmp180_a_temp, bmp180_a_press,
			bmp180_b_temp, bmp180_b_press,
			bme280_a_temp, bme280_a_press, bme280_a_humid,
			bme280_b_temp, bme280_b_press, bme280_b_humid);


		MODM_LOG_DEBUG << MODM_FILE_INFO;
		MODM_LOG_DEBUG << "Uptime: " << uptime << modm::endl;

		++uptime;
	}

	PT_END();
}
