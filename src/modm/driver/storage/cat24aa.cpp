/*
 * Copyright (c) 2020, Mike Wolfram
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "cat24aa.hpp"

modm::cat24Aa::DataTransmissionAdapter::DataTransmissionAdapter(uint8_t address) :
	I2cWriteReadTransaction(address), writeAddress(false)
{}

bool
modm::cat24Aa::DataTransmissionAdapter::configureWrite(uint16_t address,
		const uint8_t *buffer, std::size_t size)
{
	if (I2cWriteReadTransaction::configureWrite(buffer, size))
	{
		addressBuffer[0] = uint8_t(address & 0xff);
		writeAddress = true;
		return true;
	}
	return false;
}

bool
modm::cat24Aa::DataTransmissionAdapter::configureRead(uint16_t address,
		uint8_t *buffer, std::size_t size)
{
	addressBuffer[0] = uint8_t(address & 0xff);
	writeAddress = false;
	return I2cWriteReadTransaction::configureWriteRead(addressBuffer, 1, buffer, size);
}

modm::I2cTransaction::Writing
modm::cat24Aa::DataTransmissionAdapter::writing()
{
	if (writeAddress)
	{
		writeAddress = false;
		return Writing(addressBuffer, 1, OperationAfterWrite::Write);
	}
	return I2cWriteReadTransaction::writing();
}
