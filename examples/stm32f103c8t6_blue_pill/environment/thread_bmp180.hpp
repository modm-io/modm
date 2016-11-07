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

#ifndef THREAD_BMP180
#define THREAD_BMP180

#include <xpcc/processing/timer.hpp>
#include <xpcc/processing/protothread.hpp>

#include <xpcc/driver/pressure/bmp085.hpp>

#include "hardware.hpp"

class Bmp180Thread: public xpcc::pt::Protothread
{
public:
	Bmp180Thread();

	bool
	update();

	bool
	startMeasurement();

	bool
	isNewDataAvailable() {
		return new_data;
	}

	int16_t
	getTemperatureA() {
		return dataA.getTemperature();
	}

	int32_t
	getPressureA() {
		return dataA.getPressure();
	}

	int16_t
	getTemperatureB() {
		return dataB.getTemperature();
	}

	int32_t
	getPressureB() {
		return dataB.getPressure();
	}

private:
	xpcc::ShortTimeout timeout;

	xpcc::bmp085::Data dataA;
	xpcc::Bmp085<SensorsAI2cMaster> barometerA;

	xpcc::bmp085::Data dataB;
	xpcc::Bmp085<SensorsBI2cMaster> barometerB;

	bool start_measurement;
	bool new_data;
};

#endif // THREAD_BMP180
