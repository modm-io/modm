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

#ifndef MODM_LIS3MDL_HPP
#	error  "Don't include this file directly, use 'lis3mdl.hpp' instead!"
#endif

#include <cstring>

// ----------------------------------------------------------------------------
template < class I2cMaster >
modm::Lis3mdl<I2cMaster>::Lis3mdl( uint8_t address)
:	Lis3TransportI2c<I2cMaster>(address)
{
	control1Shadow.value = 0x10;
	control2Shadow.value = 0x00;
	control3Shadow.value = 0x03;
	control4Shadow.value = 0x00;
	control5Shadow.value = 0x00;
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lis3mdl<I2cMaster>::configure(DataRate rate, Scale scale)
{
	RF_BEGIN();

	DataRate_t::set(control1Shadow,rate);

	// if FAST_ODR is requested, copy the highsped modes for the Z axis
	if(control1Shadow & Control1::FAST_ODR)
	{
		HighSpeedModes hsMode = HighSpeedModes_t::get(control1Shadow);
		HighSpeedModesZ_t::set(control4Shadow,hsMode);
	}

	Scale_t::set(control2Shadow,scale);

	success = RF_CALL(this->write(static_cast<uint8_t>(Register::CTRL1),control1Shadow.value));
	if (success)
	{
		success = RF_CALL(this->write(static_cast<uint8_t>(Register::CTRL2),control2Shadow.value));
		if(success)
		{
			success = RF_CALL(this->write(static_cast<uint8_t>(Register::CTRL4),control4Shadow.value));
		}
	}
	RF_END_RETURN(success);
}


template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lis3mdl<I2cMaster>::setMode(OperationMode mode)
{
	RF_BEGIN();
	OperationMode_t::set(control3Shadow,mode);
	RF_END_RETURN_CALL(this->write(static_cast<uint8_t>(Register::CTRL3),control3Shadow.value));
}

template < class I2cMaster >
modm::lis3mdl::Scale
modm::Lis3mdl<I2cMaster>::getScale()
{
	return Scale_t::get(control2Shadow);
}

template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lis3mdl<I2cMaster>::readMagnetometerRaw(Vector3i& data)
{
	RF_BEGIN();
	success = RF_CALL(this->read(static_cast<uint8_t>(Register::OUT_X_L),reinterpret_cast<uint8_t*>(readBuffer),6));
	if(success)
	{
		data.x = readBuffer[0];
		data.y = readBuffer[1];
		data.z = readBuffer[2];
	}
	RF_END_RETURN(success);
}


template < class I2cMaster >
modm::ResumableResult<bool>
modm::Lis3mdl<I2cMaster>::readMagnetometer(Vector3f& data)
{
	RF_BEGIN();
	success = RF_CALL(this->read(static_cast<uint8_t>(Register::OUT_X_L),reinterpret_cast<uint8_t*>(readBuffer),6));

	if(success)
	{
		//use the bitmask of the scale to index the conversion  table;
		uint8_t scaleIndex = (static_cast<uint8_t>(getScale()))>>5;
		float conversionValue = convTable[scaleIndex];

		data.x = static_cast<float>(readBuffer[0]) * conversionValue;
		data.y = static_cast<float>(readBuffer[1]) * conversionValue;
		data.z = static_cast<float>(readBuffer[2]) * conversionValue;
	}

	RF_END_RETURN(success);
}