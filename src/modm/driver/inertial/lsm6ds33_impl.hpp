/*
 * Copyright (c) 2020, Benjamin Carrick
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LSM6DS33_HPP
#	error  "Don't include this file directly, use 'lsm6ds33.hpp' instead!"
#endif

#include <cstring>

// ----------------------------------------------------------------------------
template < class I2cMaster >
modm::Lsm6ds33<I2cMaster>::Lsm6ds33( uint8_t address)
:	Lis3TransportI2c<I2cMaster>(address)
{
	control1Shadow.value = 0x00;
	control2Shadow.value = 0x00;
	control3Shadow.value = 0x04;
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lsm6ds33<I2cMaster>::configureAccelerationSensor(AccDataRate accRate, AccScale accScale)
{
	RF_BEGIN();
	AccDataRate_t::set(control1Shadow,accRate);
	AccScale_t::set(control1Shadow,accScale);
	RF_END_RETURN_CALL(this->write(static_cast<uint8_t>(Register::CTRL1),control1Shadow.value));
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lsm6ds33<I2cMaster>::configureGyroscope(GyroDataRate gyroRate, GyroScale gyroScale)
{
	RF_BEGIN();
	GyroDataRate_t::set(control2Shadow,gyroRate);
	GyroScale_t::set(control2Shadow,gyroScale);
	RF_END_RETURN_CALL( this->write(static_cast<uint8_t>(Register::CTRL2),control2Shadow.value));
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lsm6ds33<I2cMaster>::readAccelerationRaw(Vector3i& acceleration)
{
	RF_BEGIN();
	success = RF_CALL(this->read(static_cast<uint8_t>(Register::OUT_X_L_XL),reinterpret_cast<uint8_t*>(readBuffer),6));
	if(success)
	{
		acceleration.x = readBuffer[0];
		acceleration.y = readBuffer[1];
		acceleration.z = readBuffer[2];
	}
	RF_END_RETURN(success);
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lsm6ds33<I2cMaster>::readGyroscopeRaw(Vector3i& spinRates)
{
	RF_BEGIN();
	success = RF_CALL(this->read(static_cast<uint8_t>(Register::OUT_X_L_G),reinterpret_cast<uint8_t*>(readBuffer),6));
	if(success)
	{
		spinRates.x = readBuffer[0];
		spinRates.y = readBuffer[1];
		spinRates.z = readBuffer[2];
	}
	RF_END_RETURN(success);
}

template < class I2cMaster >
modm::lsm6ds33::AccScale
modm::Lsm6ds33<I2cMaster>::getAccelerationScale()
{
	return AccScale_t::get(control1Shadow);
}

template < class I2cMaster >
modm::lsm6ds33::GyroScale
modm::Lsm6ds33<I2cMaster>::getGyroscopeScale()
{
	return GyroScale_t::get(control2Shadow);
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lsm6ds33<I2cMaster>::readAcceleration(Vector3f& acceleration)
{
	RF_BEGIN();
	success = RF_CALL(this->read(static_cast<uint8_t>(Register::OUT_X_L_XL),reinterpret_cast<uint8_t*>(readBuffer),6));

	if(success)
	{
		//use the bitmask of the acceleration scale to index the conversion  table;
		uint8_t accScaleIndex = (static_cast<uint8_t>(getAccelerationScale()))>>2;
		float conversionValue = accConvTable[accScaleIndex];

		acceleration.x = static_cast<float>(readBuffer[0]) * conversionValue;
		acceleration.y = static_cast<float>(readBuffer[1]) * conversionValue;
		acceleration.z = static_cast<float>(readBuffer[2]) * conversionValue;
	}

	RF_END_RETURN(success);
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lsm6ds33<I2cMaster>::readGyroscope(Vector3f& acceleration)
{
	RF_BEGIN();
	success = RF_CALL(this->read(static_cast<uint8_t>(Register::OUT_X_L_G),reinterpret_cast<uint8_t*>(readBuffer),6));

	if(success)
	{
		GyroScale scale = getGyroscopeScale();
		float conversionValue;
		//Handle the 125 dps special case
		if(scale == GyroScale::Scale_125_dps)
		{
			conversionValue = 0.003814697f;
		}
		else
		{
			//use the bitmask of the gyro scale to index the conversion table;
			uint8_t gyroScaleIndex = (static_cast<uint8_t>(scale))>>2;
			conversionValue = gyroConvTable[gyroScaleIndex];
		}

		acceleration.x = static_cast<float>(readBuffer[0]) * conversionValue;
		acceleration.y = static_cast<float>(readBuffer[1]) * conversionValue;
		acceleration.z = static_cast<float>(readBuffer[2]) * conversionValue;
	}

	RF_END_RETURN(success);
}