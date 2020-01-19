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

#ifndef MODM_CAT24AA_HPP
#	error	"Don't include this file directly, use 'cat24aa.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2cMaster>
modm::Cat24Aa<I2cMaster>::Cat24Aa() :
	I2cDevice<I2cMaster, 1, cat24Aa::DataTransmissionAdapter>(0x50)
{
}

template <typename I2cMaster>
modm::ResumableResult<bool>
modm::Cat24Aa<I2cMaster>::write(uint32_t address, const uint8_t *data,
		std::size_t length)
{
	RF_BEGIN();

	this->setAddress(this->transaction.getAddress() | ((address >> 8) & 0x07));

	RF_WAIT_UNTIL( this->transaction.configureWrite(address, data, length) and this->startTransaction() );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

template <typename I2cMaster>
modm::ResumableResult<bool>
modm::Cat24Aa<I2cMaster>::read(uint32_t address, uint8_t *data, std::size_t length)
{
	RF_BEGIN();

	this->setAddress(this->transaction.getAddress() | ((address >> 8) & 0x07));

	RF_WAIT_UNTIL( this->transaction.configureRead(address, data, length) and this->startTransaction() );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}
