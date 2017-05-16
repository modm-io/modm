/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "i2c_eeprom.hpp"

// ----------------------------------------------------------------------------
modm::i2cEeprom::DataTransmissionAdapter::DataTransmissionAdapter(uint8_t address) :
	I2cWriteReadTransaction(address), writeAddress(false)
{}

bool
modm::i2cEeprom::DataTransmissionAdapter::configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size)
{
	if (I2cWriteReadTransaction::configureWrite(buffer, size))
	{
		addressBuffer[0] = address >> 8;
		addressBuffer[1] = address;
		writeAddress = true;
		return true;
	}
	return false;
}

bool
modm::i2cEeprom::DataTransmissionAdapter::configureRead(uint16_t address, uint8_t *buffer, std::size_t size)
{
	addressBuffer[0] = address >> 8;
	addressBuffer[1] = address;
	writeAddress = false;
	return I2cWriteReadTransaction::configureWriteRead(addressBuffer, 2, buffer, size);
}

modm::I2cTransaction::Writing
modm::i2cEeprom::DataTransmissionAdapter::writing()
{
	if (writeAddress)
	{
		writeAddress = false;
		return Writing(addressBuffer, 2, OperationAfterWrite::Write);
	}
	return I2cWriteReadTransaction::writing();
}
