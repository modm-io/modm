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

#include <xpcc/processing/coroutine.hpp>
#include <xpcc/architecture/peripheral/i2c_device.hpp>

namespace xpcc
{

/**
 * I2C Eeprom
 *
 * Compatible with the 24C256 (ST) and 24FC1025 (Microchip) family and other
 * I2C eeprom with an 16-bit address pointer.
 * Base address for most 24xxyyyy eeproms is 0xA0.
 *
 * @ingroup	storage
 * @author	Fabian Greif
 * @author	Niklas Hauser
 */
template <typename I2cMaster>
class I2cEeprom : public xpcc::I2cDevice<I2cMaster>, protected xpcc::co::Coroutine
{
public:
	I2cEeprom(uint8_t address = 0xA0);

	/// Ping the device
	xpcc::co::Result<bool>
	ping(void *ctx);

	/**
	 * Write byte
	 *
	 * @param	address		Address
	 * @param	data		Data byte
	 *
	 * @return	`true`	if the data could be written,
	 * 			`false` otherwise
	 */
	inline xpcc::co::Result<bool>
	writeByte(void *ctx, uint16_t address, uint8_t data)
	{
		return write(ctx, address, &data, 1);
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
	xpcc::co::Result<bool>
	write(void *ctx, uint16_t address, const uint8_t *data, std::size_t length);

	/**
	 * Convenience function
	 *
	 * Shortcut for:
	 * @code
	 * return write(ctx, address, static_cast<const uint8_t *>(&data), sizeof(T));
	 * @endcode
	 */
	template <typename T>
	inline xpcc::co::Result<bool>
	write(void *ctx, uint16_t address, const T& data)
	{
		return write(ctx, address, static_cast<const uint8_t *>(&data), sizeof(T));
	}

	/// Read byte
	inline xpcc::co::Result<bool>
	readByte(void *ctx, uint16_t address, uint8_t &data)
	{
		return read(ctx, address, &data, 1);
	}

	/// Read block
	xpcc::co::Result<bool>
	read(void *ctx, uint16_t address, uint8_t *data, std::size_t length);

	/**
	 * Convenience function
	 *
	 * Shortcut for:
	 * @code
	 * return read(address, static_cast<uint8_t *>(&data), sizeof(T));
	 * @endcode
	 */
	template <typename T>
	inline xpcc::co::Result<bool>
	read(void *ctx, uint16_t address, T& data)
	{
		return read(ctx, address, static_cast<uint8_t *>(&data), sizeof(T));
	}

private:
	enum I2cTask : uint8_t
	{
		Idle = 0,
		Ping,
		Write,
		Read,
	};

	class DataTransmissionAdapter : public xpcc::I2cWriteReadAdapter
	{
	public:
		DataTransmissionAdapter(uint8_t address);

		bool inline
		configureWrite(uint16_t address, const uint8_t *buffer, std::size_t size);

		bool inline
		configureRead(uint16_t address, uint8_t *buffer, std::size_t size);

	protected:
		virtual Writing
		writing() override;

		uint8_t addressBuffer[2];
		bool writeAddress;
	};

	volatile uint8_t i2cTask;
	volatile uint8_t i2cSuccess;
	xpcc::I2cTagAdapter<DataTransmissionAdapter> adapter;
};

}	// namespace xpcc

#include "i2c_eeprom_impl.hpp"

#endif // XPCC_I2C_EEPROM_HPP
