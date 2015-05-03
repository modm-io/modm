// coding: utf-8
/* Copyright (c) 2014, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_LIS3_TRANSPORT_HPP
#define XPCC_LIS3_TRANSPORT_HPP

#include <xpcc/architecture/interface/spi_device.hpp>
#include <xpcc/architecture/interface/i2c_device.hpp>
#include <xpcc/processing/resumable.hpp>

namespace xpcc
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
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Lis3TransportI2c : public xpcc::I2cDevice< I2cMaster, 2 >
{
public:
	Lis3TransportI2c(uint8_t address);

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	xpcc::co::Result<bool>
	write(uint8_t reg, uint8_t value);

	/// read a 8bit value
	xpcc::co::Result<bool> ALWAYS_INLINE
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	xpcc::co::Result<bool>
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
 * @ingroup driver_inertial
 * @author	Niklas Hauser
 */
template < class SpiMaster, class Cs >
class Lis3TransportSpi : public xpcc::SpiDevice< SpiMaster >, protected xpcc::co::NestedResumable<2>
{
public:
	Lis3TransportSpi(uint8_t /*address*/);

	/// pings the sensor
	xpcc::co::Result<bool>
	ping();

protected:
	// RAW REGISTER ACCESS
	/// write a 8bit value
	xpcc::co::Result<bool>
	write(uint8_t reg, uint8_t value);

	/// read a 8bit value
	xpcc::co::Result<bool> ALWAYS_INLINE
	read(uint8_t reg, uint8_t &value)
	{
		return read(reg, &value, 1);
	}

	/// read multiple 8bit values from a start register
	xpcc::co::Result<bool>
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

} // namespace xpcc

#include "lis3_transport_impl.hpp"

#endif	// XPCC_LIS3_TRANSPORT_HPP
