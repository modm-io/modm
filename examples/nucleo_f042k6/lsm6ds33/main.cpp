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
#include <modm/driver/inertial/lsm6ds33.hpp>

using namespace Board;
using namespace std::chrono_literals;

using I2cSda	= GpioA10;
using I2cScl	= GpioA9;

int
main()
{
	Board::initialize();
	LedD13::setOutput();

	MODM_LOG_INFO << "LSM6DS33 demo" << modm::endl;

	I2cMaster1::connect<I2cSda::Sda, I2cScl::Scl>();
	I2cMaster1::initialize<SystemClock, 400_kBd>();

	// Create a sensor object with the adress of the sensor built onto the Pololu AltIMU-10 v5
	modm::Lsm6ds33<I2cMaster1> sensor(0x6B);

	// Turn on and configure the acceleration sensor
	bool accSuccess = RF_CALL_BLOCKING(sensor.configureAccelerationSensor(modm::lsm6ds33::AccDataRate::Rate_13_Hz,
																		  modm::lsm6ds33::AccScale::Scale_16_G));
	// Turn on and configure the gyroscope
	bool gyroSuccess = RF_CALL_BLOCKING(sensor.configureGyroscope(modm::lsm6ds33::GyroDataRate::Rate_13_Hz,
														 modm::lsm6ds33::GyroScale::Scale_125_dps));

	if(!(accSuccess && gyroSuccess))
	{
		MODM_LOG_INFO << "Sensor could not be configured!" << modm::endl;
	}

	modm::Vector3f accVector;
	modm::Vector3f gyroVector;

	while (true)
	{
		//Read the sensor data and print it out
		accSuccess = RF_CALL_BLOCKING(sensor.readAcceleration(accVector));
		gyroSuccess = RF_CALL_BLOCKING(sensor.readGyroscope(gyroVector));
		if(accSuccess && gyroSuccess)
		{
			MODM_LOG_INFO << "Acceleration Vector:" << modm::endl;
			MODM_LOG_INFO << "X: "<< accVector.x << " g" << modm::endl;
			MODM_LOG_INFO << "Y: "<< accVector.y << " g" << modm::endl;
			MODM_LOG_INFO << "Z: "<< accVector.z << " g" << modm::endl;
			MODM_LOG_INFO  << modm::endl;

			MODM_LOG_INFO << "Spin Rates Vector:" << modm::endl;
			MODM_LOG_INFO << "X: "<< gyroVector.x << " deg/s" << modm::endl;
			MODM_LOG_INFO << "Y: "<< gyroVector.y << " deg/s" << modm::endl;
			MODM_LOG_INFO << "Z: "<< gyroVector.z << " deg/s" << modm::endl;
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
