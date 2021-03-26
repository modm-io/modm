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

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/pressure/bmp085.hpp>

#include "hardware.hpp"

class Bmp180Thread: public modm::pt::Protothread
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
	modm::ShortTimeout timeout;

	modm::bmp085::Data dataA;
	modm::Bmp085<SensorsAI2cMaster> barometerA;

	modm::bmp085::Data dataB;
	modm::Bmp085<SensorsBI2cMaster> barometerB;

	bool start_measurement;
	bool new_data;
};

#endif // THREAD_BMP180
