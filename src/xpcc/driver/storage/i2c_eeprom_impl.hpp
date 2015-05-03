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
xpcc::I2cEeprom<I2cMaster>::I2cEeprom(uint8_t address) :
	I2cDevice<I2cMaster, 1, i2cEeprom::DataTransmissionAdapter>(address)
{
}

// MARK: - write operations

template <typename I2cMaster>
xpcc::co::ResumableResult<bool>
xpcc::I2cEeprom<I2cMaster>::write(uint16_t address, const uint8_t *data, std::size_t length)
{
	CO_BEGIN();

	CO_WAIT_UNTIL( this->transaction.configureWrite(address, data, length) and this->startTransaction() );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}

// MARK: - read operations
template <typename I2cMaster>
xpcc::co::ResumableResult<bool>
xpcc::I2cEeprom<I2cMaster>::read(uint16_t address, uint8_t *data, std::size_t length)
{
	CO_BEGIN();

	CO_WAIT_UNTIL( this->transaction.configureRead(address, data, length) and this->startTransaction() );

	CO_WAIT_WHILE( this->isTransactionRunning() );

	CO_END_RETURN( this->wasTransactionSuccessful() );
}
