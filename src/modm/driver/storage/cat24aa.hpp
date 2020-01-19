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
#define MODM_CAT24AA_HPP

#include <modm/processing/resumable.hpp>
#include <modm/architecture/interface/i2c_device.hpp>

namespace modm
{

/// @cond
struct cat24Aa {
	class DataTransmissionAdapter : public modm::I2cWriteReadTransaction
	{
	public:
		DataTransmissionAdapter(uint8_t address = 0x50);

		bool
		configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size);

		bool
		configureRead(uint16_t address, uint8_t *buffer, std::size_t size);

		inline static constexpr uint8_t getAddress() {
			return 0x50;
		}

	protected:
		virtual Writing
		writing() override;

		uint8_t addressBuffer[1];
		bool writeAddress;
	};
};
/// @endcond

/**
 * I2C Eeprom CAT24AA
 *
 * Driver for OnSemi CAT24AA Eeprom.
 * I2C eeprom with an 11-bit address pointer, encoded in 3 bits of the device
 * slave address plus the address byte.
 * Base address is fixed to 0x50.
 *
 * @ingroup	modm_driver_i2c_eeprom
 * @author	Mike Wolfram
 */
template <typename I2cMaster>
class Cat24Aa : public modm::I2cDevice<I2cMaster, 1, cat24Aa::DataTransmissionAdapter>
{
public:
	Cat24Aa();

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

} // namespace modm

#include "cat24aa_impl.hpp"

#endif // MODM_CAT24AA_HPP
