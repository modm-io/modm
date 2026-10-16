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

#include <modm/board.hpp>
#include <modm/processing.hpp>
#include <modm/driver/pressure/bme280.hpp>
#include <inttypes.h>

using I2cMaster = I2cMaster1;

modm::bme280::Data data;
modm::Bme280<I2cMaster> barometer(data, 0x76);

modm::Fiber fiber_sensor([]
{
	MODM_LOG_INFO << "Ping the BME280" << modm::endl;
	while (not barometer.ping()) modm::this_fiber::sleep_for(100ms);

	MODM_LOG_INFO << "Initialize the BME280" << modm::endl;
	while (not barometer.initialize()) modm::this_fiber::sleep_for(100ms);

	while (true)
	{
		// Returns when new data was read from the sensor
		barometer.readout();

		const int32_t temperature = data.getTemperature();	// 0.01 °C
		const int32_t pressure = data.getPressure();		// 0.001 Pa
		const int32_t humidity = data.getHumidity();		// 1/1024 %RH
		MODM_LOG_INFO.printf("T = %" PRId32 ".%02" PRId32 " C, P = %" PRId32 " Pa, H = %" PRId32 " %%RH\n",
				temperature / 100, temperature % 100, pressure / 1000, humidity / 1024);

		Board::LedGreen::toggle();
		modm::this_fiber::sleep_for(1s);
	}
});

int
main()
{
	Board::initialize();

	I2cMaster::connect<GpioB6::Scl, GpioB7::Sda>();
	I2cMaster::initialize<Board::SystemClock, 100_kHz>();

	MODM_LOG_INFO << "BME280 environment sensor example" << modm::endl;

	modm::fiber::Scheduler::run();
	return 0;
}
