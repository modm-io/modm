/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_HMC58X3_HPP
#	error "Don't include this file directly, use 'hmc58x3.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Hmc58x3<I2cMaster>::Hmc58x3(Data &data, uint8_t address)
:	I2cDevice<I2cMaster>(address), data(data),
	rawBuffer{0x10, 0x20, 0x01, 0,0,0,0,0,0, 0x00}
{
}

// MARK: - Tasks
template < typename I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::readMagneticField()
{
	if (read(Register::DataX_Msb, rawBuffer+3, 7))
	{
		std::memcpy(data.data, rawBuffer+3, 6);
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
// MARK: - base methods
template < typename I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::configureRaw(uint8_t rate, uint8_t gain, const uint8_t* gainValues, uint8_t average)
{
	rawBuffer[0] = rate | average;
	rawBuffer[1] = gain;
	rawBuffer[2] = uint8_t(OperationMode::ContinousConversion);

	if (write(Register::ConfigA, rawBuffer, 3))
	{
		// look-up the gain for the data object
		data.gain = gainValues[((gain >> 5) & 0x07)];
		return true;
	}

	return false;
}

template < typename I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::setGainRaw(uint8_t gain, const uint8_t* gainValues)
{
	rawBuffer[1] = (rawBuffer[1] & ~uint8_t(ConfigB::GN_Mask)) | gain;

	if (write(Register::ConfigB, rawBuffer[1]))
	{
		// look-up the gain for the data object
		data.gain = gainValues[((gain >> 5) & 0x07)];
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------
// MARK: - register access
template < typename I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::readStatus()
{
	return read(Register::Status, rawBuffer[9]);
}

// MARK: update register
template < typename I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask)
{
	rawBuffer[index] = (rawBuffer[index] & ~clearMask) | setMask;

	return write(Register(index), rawBuffer[index]);
}

// MARK: write multilength register
template < class I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::write(Register reg, uint8_t *buffer, uint8_t length)
{
	if (length > 5)
		return false;

	rawBuffer[3] = uint8_t(reg);
	std::memcpy(rawBuffer+4, buffer, length);
	return I2cDevice<I2cMaster>::write(rawBuffer+3, length+1);
}

// MARK: read multilength register
template < class I2cMaster >
bool
modm::Hmc58x3<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	rawBuffer[3] = uint8_t(reg);
	return I2cDevice<I2cMaster>::writeRead(rawBuffer+3, 1, buffer, length);
}
