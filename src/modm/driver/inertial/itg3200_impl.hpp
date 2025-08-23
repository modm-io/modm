/*
 * Copyright (c) 2012-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_ITG3200_HPP
#	error "Don't include this file directly, use 'itg3200.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template < typename I2cMaster >
modm::Itg3200<I2cMaster>::Itg3200(Data &data, uint8_t address)
:	I2cDevice<I2cMaster>(address), data(data),
	rawBuffer{0x00, 0x00, 0x00, 0,0,0,0,0,0,0,0, 0x00}
{
}

// MARK: - Tasks
template < typename I2cMaster >
bool
modm::Itg3200<I2cMaster>::configure(LowPassFilter filter, uint8_t divider)
{
	rawBuffer[4] = divider;
	rawBuffer[5] = rawBuffer[0] = LowPassFilter_t(filter).value | uint8_t(Filter::FullScale);
	rawBuffer[6] = rawBuffer[1] = 0;

	return write(Register::SMPLRT_DIV, rawBuffer, 3, false);
}

template < typename I2cMaster >
bool
modm::Itg3200<I2cMaster>::readRotation()
{
	if (read(Register::INT_STATUS, rawBuffer+2, 9))
	{
		std::memcpy(data.data, rawBuffer+3, 8);
		return true;
	}

	return false;
}

template < typename I2cMaster >
bool
modm::Itg3200<I2cMaster>::setSampleRateDivider(uint8_t divider)
{
	rawBuffer[4] = divider;

	return write(Register::SMPLRT_DIV, rawBuffer+4, 1, false);
}

// ----------------------------------------------------------------------------
// MARK: - register access
template < typename I2cMaster >
bool
modm::Itg3200<I2cMaster>::readStatus()
{
	return read(Register::INT_STATUS, rawBuffer[2]);
}

// MARK: update register
template < typename I2cMaster >
bool
modm::Itg3200<I2cMaster>::updateRegister(uint8_t index, uint8_t setMask, uint8_t clearMask)
{
	rawBuffer[index] = (rawBuffer[index] & ~clearMask) | setMask;

	return write(Register(index), rawBuffer[index]);
}

// MARK: write multilength register
template < class I2cMaster >
bool
modm::Itg3200<I2cMaster>::write(Register reg, uint8_t *buffer, uint8_t length, bool copyBuffer)
{
	if (length > 7)
		return false;

	rawBuffer[3] = uint8_t(reg);
	if (copyBuffer) std::memcpy(rawBuffer+4, buffer, length);

	this->transaction.configureWrite(rawBuffer+3, length+1);

	return this->runTransaction();
}

// MARK: read multilength register
template < class I2cMaster >
bool
modm::Itg3200<I2cMaster>::read(Register reg, uint8_t *buffer, uint8_t length)
{
	rawBuffer[3] = uint8_t(reg);
	this->transaction.configureWriteRead(rawBuffer+3, 1, buffer, length);

	return this->runTransaction();
}
