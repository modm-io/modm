// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_I2C_EEPROM_HPP
#define XPCC_I2C_EEPROM_HPP

#include <xpcc/processing/resumable.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>

namespace xpcc
{

/// @cond
struct i2cEeprom
{
	class DataTransmissionAdapter : public xpcc::I2cWriteReadTransaction
	{
	public:
		DataTransmissionAdapter(uint8_t address);

		bool
		configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size);

		bool
		configureRead(uint16_t address, uint8_t *buffer, std::size_t size);

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
 * Base address for most 24xxyyyy eeproms is 0xA0.
 *
 * @ingroup	driver_storage
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */
template <typename I2cMaster>
class I2cEeprom : public xpcc::I2cDevice< I2cMaster, 1, i2cEeprom::DataTransmissionAdapter >
{
public:
	I2cEeprom(uint8_t address = 0xA0);

	/**
	 * Write byte
	 *
	 * @param	address		Address
	 * @param	data		Data byte
	 *
	 * @return	`true`	if the data could be written,
	 * 			`false` otherwise
	 */
	inline xpcc::ResumableResult<bool>
	writeByte(uint16_t address, uint8_t data)
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
	xpcc::ResumableResult<bool>
	write(uint16_t address, const uint8_t *data, std::size_t length);

	/**
	 * Convenience function
	 *
	 * Shortcut for:
	 * @code
	 * return write(address, static_cast<const uint8_t *>(&data), sizeof(T));
	 * @endcode
	 */
	template <typename T>
	inline xpcc::ResumableResult<bool>
	write(uint16_t address, const T& data)
	{
		return write(address, static_cast<const uint8_t *>(&data), sizeof(T));
	}

	/// Read byte
	inline xpcc::ResumableResult<bool>
	readByte(uint16_t address, uint8_t &data)
	{
		return read(address, &data, 1);
	}

	/// Read block
	xpcc::ResumableResult<bool>
	read(uint16_t address, uint8_t *data, std::size_t length);

	/**
	 * Convenience function
	 *
	 * Shortcut for:
	 * @code
	 * return read(address, static_cast<uint8_t *>(&data), sizeof(T));
	 * @endcode
	 */
	template <typename T>
	inline xpcc::ResumableResult<bool>
	read(uint16_t address, T& data)
	{
		return read(address, static_cast<uint8_t *>(&data), sizeof(T));
	}
};

}	// namespace xpcc

#include "i2c_eeprom_impl.hpp"

#endif // XPCC_I2C_EEPROM_HPP
