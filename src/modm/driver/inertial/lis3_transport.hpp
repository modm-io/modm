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

#ifndef MODM_LIS3_TRANSPORT_HPP
#define MODM_LIS3_TRANSPORT_HPP

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/resumable.hpp>

namespace modm
{

/**
 * LIS3xx I2C Transport Layer.
 *
 * This class manages communication with the accelerometer via the I2C bus.
 *
 * To enable I2C mode in the LIS3xx devices, the CS pin must be tied high.
 * However, it is internally pulled-up, so just leaving the CS pin disconnected should work too.
 *
 * The I2C interface is compliant with Fast Mode (up to 400kHz).
 *
 * @see Lis302dl
 * @see Lis3dsh
 *
 * @ingroup modm_driver_lis3_transport
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Lis3TransportI2c : public modm::I2cDevice< I2cMaster, 2 >
{
public:
	Lis3TransportI2c(uint8_t address);

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	/// read a 8bit value
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t *buffer, uint8_t length);

	// increment address or not?
	/// @cond
	static constexpr uint8_t AddressIncrement = 0x80;
	static constexpr uint8_t AddressStatic = 0x00;
	/// @endcond

private:
	uint8_t buffer[2];
};

/**
 * LIS3xx SPI Transport Layer.
 *
 * This class manages communication with the accelerometer via the SPI bus.
 * The SPI interface can be clocked with up to 10MHz and requires Mode3.
 *
 * @see Lis302dl
 * @see Lis3dsh
 *
 * @tparam	Cs	connected Chip Select Pin
 *
 * @ingroup modm_driver_lis3_transport
 * @author	Niklas Hauser
 */
template < class SpiMaster, class Cs >
class Lis3TransportSpi : public modm::SpiDevice< SpiMaster >, protected modm::NestedResumable<2>
{
public:
	Lis3TransportSpi(uint8_t /*address*/);

	/// pings the sensor
	modm::ResumableResult<bool>
	ping();

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	modm::ResumableResult<bool>
	write(uint8_t reg, uint8_t value);

	/// read a 8bit value
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	modm::ResumableResult<bool>
	read(uint8_t reg, uint8_t *buffer, uint8_t length);

	// increment address or not?
	/// @cond
	static constexpr uint8_t AddressIncrement = 0x40;
	static constexpr uint8_t AddressStatic = 0x00;
	/// @endcond

private:
	uint8_t whoAmI;

	// write read bit on the address
	static constexpr uint8_t Read = 0x80;
	static constexpr uint8_t Write = 0x00;
};

} // namespace modm

#include "lis3_transport_impl.hpp"

#endif	// MODM_LIS3_TRANSPORT_HPP
