/*
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2011, Georgi Grinshpun
 * Copyright (c) 2012-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_I2C_EEPROM_HPP
#define MODM_I2C_EEPROM_HPP

#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/// @cond
struct i2cEeprom
{
	class DataTransmissionAdapter : public modm::I2cWriteReadTransaction
	{
	public:
		DataTransmissionAdapter(uint8_t address);

		bool
		configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size);

		bool
		configureRead(uint16_t address, uint8_t *buffer, std::size_t size);

		inline uint8_t getAddress() const {
			return this->address >> 1;
		}

	protected:
		virtual Writing
		writing() override;

		uint8_t addressBuffer[2];
		bool writeAddress;
	};
};
/// @endcond

/**
 * I2C Eeprom
 *
 * Compatible with the 24C256 (ST) and 24FC1025 (Microchip) family and other
 * I2C eeprom with an 16-bit address pointer.
 * Base address for most 24xxyyyy eeproms is 0x50.
 *
 * @ingroup	driver_storage
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */
template <typename I2cMaster>
class I2cEeprom : public modm::I2cDevice< I2cMaster, 1, i2cEeprom::DataTransmissionAdapter >
{
public:
	I2cEeprom(uint8_t address = 0x50);

	/**
	 * Write byte
	 *
	 * @param	address		Address
	 * @param	data		Data byte
	 *
	 * @return	`true`	if the data could be written,
	 * 			`false` otherwise
	 */
	inline modm::ResumableResult<bool>
	writeByte(uint32_t address, uint8_t data)
	{
		return write(address, &data, 1);
	}

	/**
	 * Write block
	 *
	 * @param	address		Address
	 * @param	data		Data block
	 * @param	length		Number of bytes to be written
	 *
	 * @return	`true`	if the data could be written,
	 * 			`false` otherwise
	 */
	modm::ResumableResult<bool>
	write(uint32_t address, const uint8_t *data, std::size_t length);

	/**
	 * Convenience function
	 *
	 * Shortcut for:
	 * @code
	 * return write(address, static_cast<const uint8_t *>(&data), sizeof(T));
	 * @endcode
	 */
	template <typename T>
	inline modm::ResumableResult<bool>
	write(uint32_t address, const T& data)
	{
		return write(address, reinterpret_cast<const uint8_t *>(&data), sizeof(T));
	}

	/// Read byte
	inline modm::ResumableResult<bool>
	readByte(uint32_t address, uint8_t &data)
	{
		return read(address, &data, 1);
	}

	/// Read block
	modm::ResumableResult<bool>
	read(uint32_t address, uint8_t *data, std::size_t length);

	/**
	 * Convenience function
	 *
	 * Shortcut for:
	 * @code
	 * return read(address, static_cast<uint8_t *>(&data), sizeof(T));
	 * @endcode
	 */
	template <typename T>
	inline modm::ResumableResult<bool>
	read(uint16_t address, T& data)
	{
		return read(address, reinterpret_cast<uint8_t *>(&data), sizeof(T));
	}
};

}	// namespace modm

#include "i2c_eeprom_impl.hpp"

#endif // MODM_I2C_EEPROM_HPP
