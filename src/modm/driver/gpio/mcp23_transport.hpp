/*
 * Copyright (c) 2014-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP23_TRANSPORT_HPP
#define MODM_MCP23_TRANSPORT_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/**
 * MCP23xxx I2C Transport Layer.
 *
 * This class manages communication with the gpio expander via the I2C bus.
 *
 * The I2C interface is compliant with Fast Mode (up to 400kHz).
 *
 * @see modm::Mcp23x08
 * @see modm::Mcp23x17
 *
 * @ingroup modm_driver_mcp23x17
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Mcp23TransportI2c : public modm::I2cDevice< I2cMaster, 2 >
{
public:
	Mcp23TransportI2c(uint8_t address);

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	/// write a 16bit value
	modm::ResumableResult<bool>
	write16(uint8_t reg, uint16_t value);

	/// read a 8bit value
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	modm::ResumableResult<bool>
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
 * @ingroup modm_driver_mcp23x17
 * @author	Niklas Hauser
 */
template < class SpiMaster, class Cs >
class Mcp23TransportSpi : public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<2>
{
public:
	Mcp23TransportSpi(uint8_t address);

	/// pings the sensor
	modm::ResumableResult<bool>
	ping();

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	/// write a 16bit value
	modm::ResumableResult<bool>
	write16(uint8_t reg, uint16_t value);

	/// read a 8bit value
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t *buffer, uint8_t length);

private:
	uint8_t address;

	// write read bit on the address
	static constexpr uint8_t Read = 0x01;
	static constexpr uint8_t Write = 0x00;
};

} // namespace modm

#include "mcp23_transport_impl.hpp"

#endif	// MODM_MCP23_TRANSPORT_HPP
