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

#ifndef THREAD_BME280
#define THREAD_BME280

#include <modm/processing/timer.hpp>
#include <modm/processing/protothread.hpp>

#include <modm/driver/pressure/bme280.hpp>

#include "hardware.hpp"

class Bme280Thread: public modm::pt::Protothread
{
public:
	Bme280Thread();

	bool
	update();

	bool
	startMeasurement();

	bool
	isNewDataAvailable() {
		return new_data;
	}

	int32_t
	getTemperatureA() {
		return dataA.getTemperature();
	}

	int32_t
	getPressureA() {
		return dataA.getPressure();
	}

	int32_t
	getHumidityA() {
		return dataA.getHumidity();
	}

	int32_t
	getTemperatureB() {
		return dataB.getTemperature();
	}

	int32_t
	getPressureB() {
		return dataB.getPressure();
	}

	int32_t
	getHumidityB() {
		return dataB.getHumidity();
	}

private:
	modm::ShortTimeout timeout;

	modm::bme280::Data dataA;
	modm::Bme280<SensorsAI2cMaster> barometerA;

	modm::bme280::Data dataB;
	modm::Bme280<SensorsBI2cMaster> barometerB;

	bool start_measurement;
	bool new_data;
};

#endif // THREAD_BME280
