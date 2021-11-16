/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2016, Niklas Hauser
 * Copyright (c) 2021, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_I2C_EEPROM_HPP
#	error	"Don't include this file directly, use 'i2c_eeprom.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename I2cMaster, size_t AddressBytes>
modm::I2cEeprom<I2cMaster, AddressBytes>::I2cEeprom(uint8_t address) :
	I2cDevice<I2cMaster, 1, i2c_eeprom::detail::DataTransmissionAdapter<AddressBytes>>{address}
{
}

// MARK: - write operations
template <typename I2cMaster, size_t AddressBytes>
modm::ResumableResult<bool>
modm::I2cEeprom<I2cMaster, AddressBytes>::write(uint32_t address, const uint8_t *data, std::size_t length)
{
	RF_BEGIN();

	if constexpr (AddressBytes > 1) {
		if (address & 0x10000) this->setAddress(this->transaction.getAddress() | 0b100);
		else this->setAddress(this->transaction.getAddress() & ~0b100);
	} else {
		this->setAddress(this->transaction.getAddress());
	}

	RF_WAIT_UNTIL( this->transaction.configureWrite(address, data, length) and this->startTransaction() );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

// MARK: - read operations
template <typename I2cMaster, size_t AddressBytes>
modm::ResumableResult<bool>
modm::I2cEeprom<I2cMaster, AddressBytes>::read(uint32_t address, uint8_t *data, std::size_t length)
{
	RF_BEGIN();

	if constexpr (AddressBytes > 1) {
		if (address & 0x10000) this->setAddress(this->transaction.getAddress() | 0b100);
		else this->setAddress(this->transaction.getAddress() & ~0b100);
	} else {
		this->setAddress(this->transaction.getAddress());
	}

	RF_WAIT_UNTIL( this->transaction.configureRead(address, data, length) and this->startTransaction() );

	RF_WAIT_WHILE( this->isTransactionRunning() );

	RF_END_RETURN( this->wasTransactionSuccessful() );
}

namespace modm::i2c_eeprom::detail
{

template<size_t AddressBytes>
DataTransmissionAdapter<AddressBytes>::DataTransmissionAdapter(uint8_t address) :
	I2cWriteReadTransaction(address), writeAddress(false)
{}

template<size_t AddressBytes>
bool
DataTransmissionAdapter<AddressBytes>::configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size)
{
	if (I2cWriteReadTransaction::configureWrite(buffer, size))
	{
		if constexpr (AddressBytes == 1) {
			addressBuffer[0] = address;
		} else {
			addressBuffer[0] = address >> 8;
			addressBuffer[1] = address;
		}
		writeAddress = true;
		return true;
	}
	return false;
}

template<size_t AddressBytes>
bool
DataTransmissionAdapter<AddressBytes>::configureRead(uint16_t address, uint8_t *buffer, std::size_t size)
{
	if constexpr (AddressBytes == 1) {
		addressBuffer[0] = address;
	} else {
		addressBuffer[0] = address >> 8;
		addressBuffer[1] = address;
	}
	writeAddress = false;
	return I2cWriteReadTransaction::configureWriteRead(addressBuffer, AddressBytes, buffer, size);
}

template<size_t AddressBytes>
I2cTransaction::Writing
DataTransmissionAdapter<AddressBytes>::writing()
{
	if (writeAddress)
	{
		writeAddress = false;
		return Writing(addressBuffer, AddressBytes, OperationAfterWrite::Write);
	}
	return I2cWriteReadTransaction::writing();
}

}
