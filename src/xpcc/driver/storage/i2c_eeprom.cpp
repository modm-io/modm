// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include "i2c_eeprom.hpp"

// ----------------------------------------------------------------------------
xpcc::i2cEeprom::DataTransmissionAdapter::DataTransmissionAdapter(uint8_t address) :
	I2cWriteReadTransaction(address), writeAddress(false)
{}

bool
xpcc::i2cEeprom::DataTransmissionAdapter::configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size)
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
xpcc::i2cEeprom::DataTransmissionAdapter::configureRead(uint16_t address, uint8_t *buffer, std::size_t size)
{
	addressBuffer[0] = address >> 8;
	addressBuffer[1] = address;
	writeAddress = false;
	return I2cWriteReadTransaction::configureWriteRead(addressBuffer, 2, buffer, size);
}

xpcc::I2cTransaction::Writing
xpcc::i2cEeprom::DataTransmissionAdapter::writing()
{
	if (writeAddress)
	{
		writeAddress = false;
		return Writing(addressBuffer, 2, OperationAfterWrite::Write);
	}
	return I2cWriteReadTransaction::writing();
}
