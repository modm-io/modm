/*
 * Copyright (c) 2023, Vivien Henry
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/pressure/ms5837.hpp>


using namespace Board;
using namespace std::chrono_literals;

using I2cSda	= GpioA10;
using I2cScl	= GpioA9;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	MODM_LOG_INFO << "MS5837 demo" << modm::endl;

	I2cMaster1::connect<I2cSda::Sda, I2cScl::Scl>();
	I2cMaster1::initialize<SystemClock, 100_kBd>();

	// Create a sensor object
	modm::ms5837data::Data sensor_data;
	modm::Ms5837<I2cMaster1> sensor(sensor_data);

	int32_t press, temp;
	float press_f, temp_f;

	// Turn on and configure the pressure sensor
	bool success = RF_CALL_BLOCKING(sensor.initialize());

	if(!success)
	{
		MODM_LOG_DEBUG << "MS5837 Initialition failed" << modm::endl;
	}

	while (true)
	{
		//Read the sensor data and print it out
		success = RF_CALL_BLOCKING(sensor.readout());

		if(success)
		{
			press = sensor_data.getPressure(); // int32_t
			//press_f = sensor_data.getPressure(); // float
			sensor_data.getPressure(press_f); // float via reference

			MODM_LOG_INFO << "Pressure: (tenth of mbar): " << press << " and in mbar (float)" << press_f << modm::endl;

			temp = sensor_data.getTemperature();
			//temp_f = sensor_data.getTemperature();
			sensor_data.getTemperature(temp_f);

			MODM_LOG_INFO << "Temp: (0.01°C): " << temp << " and °C" << temp_f << modm::endl;
		}
		else
		{
			MODM_LOG_INFO << "Sensor could not be read!" << modm::endl;
		}
		modm::delay(1s);
	}
	return 0;
}
