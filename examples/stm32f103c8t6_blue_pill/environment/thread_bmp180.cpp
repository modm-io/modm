/*
 * Copyright (c) 2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <xpcc/architecture/platform.hpp>
#include <xpcc/debug/logger.hpp>

#include "thread_bmp180.hpp"

// ----------------------------------------------------------------------------
// Set the log level
#undef	XPCC_LOG_LEVEL
#define	XPCC_LOG_LEVEL xpcc::log::INFO

// ----------------------------------------------------------------------------
Bmp180Thread::Bmp180Thread() :
	barometerA(dataA, 0x77),
	barometerB(dataB, 0x77),
	start_measurement(false),
	new_data(false)
{
}

bool
Bmp180Thread::startMeasurement()
{
	start_measurement = true;
	new_data = false;
	return true;
}

bool
Bmp180Thread::update()
{
	PT_BEGIN();

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "Ping the Barometer BMP180" << xpcc::endl;

	// ping the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerA.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerB.ping()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "Barometer BMP180 responded" << xpcc::endl;

	// Configure the device until it responds
	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerA.initialize()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	while(true)
	{
		// we wait until the task started
		if (PT_CALL(barometerB.initialize()))
			break;
		// otherwise, try again in 100ms
		this->timeout.restart(100);
		PT_WAIT_UNTIL(this->timeout.isExpired());
	}

	XPCC_LOG_DEBUG << XPCC_FILE_INFO;
	XPCC_LOG_DEBUG << "BMP180 configured" << xpcc::endl;

	// static xpcc::bmp085::Calibration &cal = data.getCalibration();

	// XPCC_LOG_DEBUG << "Calibration data is: \n";
	// XPCC_LOG_DEBUG.printf(" ac1 %d\n", cal.ac1);
	// XPCC_LOG_DEBUG.printf(" ac2 %d\n", cal.ac2);
	// XPCC_LOG_DEBUG.printf(" ac3 %d\n", cal.ac3);
	// XPCC_LOG_DEBUG.printf(" ac4 %d\n", cal.ac4);
	// XPCC_LOG_DEBUG.printf(" ac5 %d\n", cal.ac5);
	// XPCC_LOG_DEBUG.printf(" ac6 %d\n", cal.ac6);
	// XPCC_LOG_DEBUG.printf(" b1 %d\n", cal.b1);
	// XPCC_LOG_DEBUG.printf(" b2 %d\n", cal.b2);
	// XPCC_LOG_DEBUG.printf(" mb %d\n", cal.mb);
	// XPCC_LOG_DEBUG.printf(" mc %d\n", cal.mc);
	// XPCC_LOG_DEBUG.printf(" md %d\n", cal.md);

	while (true)
	{
		PT_WAIT_UNTIL(start_measurement);

		// Returns when new data was read from the sensor
		PT_CALL(barometerA.readout());
		PT_CALL(barometerB.readout());
		new_data = true;

		{
			auto temp = dataA.getTemperature();
			auto press = dataA.getPressure();

			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BMP180: Calibrated temperature in 0.1 degree Celsius is : %d\n", temp  );
			XPCC_LOG_DEBUG << XPCC_FILE_INFO;
			XPCC_LOG_DEBUG.printf("BMP180: Calibrated pressure in Pa is                    : %d\n", press );
		}

		start_measurement = false;
	}

	PT_END();
}
