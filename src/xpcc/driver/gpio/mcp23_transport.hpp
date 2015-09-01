// coding: utf-8
/* Copyright (c) 2015, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_MCP23_TRANSPORT_HPP
#define XPCC_MCP23_TRANSPORT_HPP

#include <xpcc/architecture/interface/spi_device.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/resumable.hpp>

namespace xpcc
{

/**
 * MCP23xxx I2C Transport Layer.
 *
 * This class manages communication with the gpio expander via the I2C bus.
 *
 * The I2C interface is compliant with Fast Mode (up to 400kHz).
 *
 * @see xpcc::Mcp23x08
 * @see xpcc::Mcp23x17
 *
 * @ingroup driver_gpio
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Mcp23TransportI2c : public xpcc::I2cDevice< I2cMaster, 2 >
{
public:
	Mcp23TransportI2c(uint8_t address);

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	xpcc::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	/// write a 16bit value
	xpcc::ResumableResult<bool>
	write16(uint8_t reg, uint16_t value);

	/// read a 8bit value
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	xpcc::ResumableResult<bool>
	read(uint8_t reg, uint8_t *buffer, uint8_t length);

private:
	uint8_t buffer[2];
};

/**
 * MCP23xxx SPI Transport Layer.
 *
 * This class manages communication with the gpio expcander via the SPI bus.
 * The SPI interface can be clocked with up to 10MHz and requires Mode3.
 *
 * @see Mcp23x08
 * @see Mcp23x17
 *
 * @tparam	Cs	connected Chip Select Pin
 *
 * @ingroup driver_gpio
 * @author	Niklas Hauser
 */
template < class SpiMaster, class Cs >
class Mcp23TransportSpi : public xpcc::SpiDevice< SpiMaster >, protected xpcc::NestedResumable<2>
{
public:
	Mcp23TransportSpi(uint8_t address);

	/// pings the sensor
	xpcc::ResumableResult<bool>
	ping();

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	xpcc::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	/// write a 16bit value
	xpcc::ResumableResult<bool>
	write16(uint8_t reg, uint16_t value);

	/// read a 8bit value
	xpcc::ResumableResult<bool> ALWAYS_INLINE
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	xpcc::ResumableResult<bool>
	read(uint8_t reg, uint8_t *buffer, uint8_t length);

private:
	uint8_t address;

	// write read bit on the address
	static constexpr uint8_t Read = 0x01;
	static constexpr uint8_t Write = 0x00;
};

} // namespace xpcc

#include "mcp23_transport_impl.hpp"

#endif	// XPCC_MCP23_TRANSPORT_HPP
