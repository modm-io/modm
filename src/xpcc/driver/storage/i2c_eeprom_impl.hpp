/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, Dave Webb
 * Copyright (c) 2012-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
xpcc::ResumableResult<bool>
xpcc::I2cEeprom<I2cMaster>::write(uint32_t address, const uint8_t *data, std::size_t length)
{
	RF_BEGIN();

	if (address & 0x10000) this->setAddress(this->transaction.getAddress() | 0b100);
	else this->setAddress(this->transaction.getAddress() & ~0b100);

	RF_WAIT_UNTIL( this->transaction.configureWrite(address, data, length) and this->startTransaction() );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

// MARK: - read operations
template <typename I2cMaster>
xpcc::ResumableResult<bool>
xpcc::I2cEeprom<I2cMaster>::read(uint32_t address, uint8_t *data, std::size_t length)
{
	RF_BEGIN();

	if (address & 0x10000) this->setAddress(this->transaction.getAddress() | 0b100);
	else this->setAddress(this->transaction.getAddress() & ~0b100);

	RF_WAIT_UNTIL( this->transaction.configureRead(address, data, length) and this->startTransaction() );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}
