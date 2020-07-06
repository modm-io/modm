/*
 * Copyright (c) 2020, Benjamin Carrick
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <modm/board.hpp>
#include <modm/driver/inertial/lis3mdl.hpp>

using namespace Board;
using namespace std::chrono_literals;

using I2cSda	= GpioA10;
using I2cScl	= GpioA9;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	MODM_LOG_INFO << "LIS3MDL demo" << modm::endl;

	I2cMaster1::connect<I2cSda::Sda, I2cScl::Scl>();
	I2cMaster1::initialize<SystemClock, 400_kBd>();

	// Create a sensor object with the adress of the sensor built onto the Pololu AltIMU-10 v5
	modm::Lis3mdl<I2cMaster1> sensor(0x1E);

	// Turn on and configure the magnetometer
	bool success = RF_CALL_BLOCKING(sensor.configure(modm::lis3mdl::DataRate::Rate_5_Hz,
													 modm::lis3mdl::Scale::Scale_8_gauss));


	if(!success)
	{
		MODM_LOG_INFO << "Sensor could not be configured!" << modm::endl;
	}

	// Set the sensor to continous acquistion and turn on the temperature sensing
	success = RF_CALL_BLOCKING(sensor.setMode(modm::lis3mdl::OperationMode::Continous));
	if(!success)
	{
		MODM_LOG_INFO << "Sensor could not be started!" << modm::endl;
	}

	modm::Vector3f magVector;

	while (true)
	{
		//Read the sensor data and print it out
		success = RF_CALL_BLOCKING(sensor.readMagnetometer(magVector));

		if(success)
		{
			MODM_LOG_INFO << "Magnetic Vector:" << modm::endl;
			MODM_LOG_INFO << "X: "<< magVector.x << " gauss" << modm::endl;
			MODM_LOG_INFO << "Y: "<< magVector.y << " gauss" << modm::endl;
			MODM_LOG_INFO << "Z: "<< magVector.z << " gauss" << modm::endl;
			MODM_LOG_INFO  << modm::endl;
		}
		else
		{
			MODM_LOG_INFO << "Sensor could not be read!" << modm::endl;
		}
		modm::delay(1s);
	}
	return 0;
}
