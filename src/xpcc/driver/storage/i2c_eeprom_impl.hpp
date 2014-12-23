// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C_EEPROM_HPP
#	error	"Don't include this file directly, use 'i2c_eeprom.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2cMaster>
xpcc::I2cEeprom<I2cMaster>::I2cEeprom(uint8_t address)
:	i2cTask(I2cTask::Idle), i2cSuccess(0), adapter(address, i2cTask, i2cSuccess)
{
}

// // MARK: - write operations
template <typename I2cMaster>
xpcc::co::Result<bool>
xpcc::I2cEeprom<I2cMaster>::ping(void *ctx)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configurePing() and
			(i2cTask = I2cTask::Ping, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Ping);

	CO_END_RETURN(i2cSuccess == I2cTask::Ping);
}

template <typename I2cMaster>
xpcc::co::Result<bool>
xpcc::I2cEeprom<I2cMaster>::write(void *ctx, uint16_t address, const uint8_t *data, std::size_t length)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configureWrite(address, data, length) and
			(i2cTask = I2cTask::Write, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Write);

	CO_END_RETURN(i2cSuccess == I2cTask::Write);
}

// MARK: - read operations
template <typename I2cMaster>
xpcc::co::Result<bool>
xpcc::I2cEeprom<I2cMaster>::read(void *ctx, uint16_t address, uint8_t *data, std::size_t length)
{
	CO_BEGIN(ctx);

	CO_WAIT_UNTIL(adapter.configureRead(address, data, length) and
			(i2cTask = I2cTask::Read, this->startTransaction(&adapter)));

	CO_WAIT_WHILE(i2cTask == I2cTask::Read);

	CO_END_RETURN(i2cSuccess == I2cTask::Read);
}

// ----------------------------------------------------------------------------
// MARK: DataTransmissionAdapter
template < class I2cMaster >
xpcc::I2cEeprom<I2cMaster>::DataTransmissionAdapter::DataTransmissionAdapter(uint8_t address)
:	I2cWriteReadAdapter(address), writeAddress(false)
{}

template < class I2cMaster >
bool
xpcc::I2cEeprom<I2cMaster>::DataTransmissionAdapter::configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size)
{
	if (I2cWriteReadAdapter::configureWrite(buffer, size))
	{
		addressBuffer[0] = address >> 8;
		addressBuffer[1] = address;
		writeAddress = true;
		return true;
	}
	return false;
}

template < class I2cMaster >
bool
xpcc::I2cEeprom<I2cMaster>::DataTransmissionAdapter::configureRead(uint16_t address, uint8_t *buffer, std::size_t size)
{
	addressBuffer[0] = address >> 8;
	addressBuffer[1] = address;
	writeAddress = false;
	return I2cWriteReadAdapter::configureWriteRead(addressBuffer, 2, buffer, size);
}

template < class I2cMaster >
xpcc::I2cTransaction::Writing
xpcc::I2cEeprom<I2cMaster>::DataTransmissionAdapter::writing()
{
	if (writeAddress)
	{
		writeAddress = false;
		return Writing(addressBuffer, 2, OperationAfterWrite::Write);
	}
	return I2cWriteReadAdapter::writing();
}
