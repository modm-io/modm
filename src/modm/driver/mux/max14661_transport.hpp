/*
 * Copyright (c) 2026, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/spi_device.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/fiber.hpp>

namespace modm
{

/**
 * MAX14661 I2C Transport Layer.
 *
 * In I2C mode the switches of each bank are updated through the shadow
 * registers: the desired 16-bit channel mask is written into the two
 * shadow registers of a bank, and then the "copy shadow registers to
 * switches" command is issued for that bank. There is no command that
 * copies both banks at once, so bank A and bank B always have to be
 * updated with two separate register writes.
 *
 * The I2C interface is compliant with Fast Mode (up to 400kHz). The slave
 * address is configured by the A0/A1 pins.
 *
 * @see Max14661
 *
 * @ingroup modm_driver_max14661
 * @author	Niklas Hauser
 */
template < class I2cMaster >
class Max14661TransportI2c : public modm::I2cDevice< I2cMaster >
{
public:
	Max14661TransportI2c(uint8_t address) :	I2cDevice<I2cMaster>(address) {}

protected:
	/// Write the 16-bit channel mask into the bank A shadow registers
	/// and copy them to the bank A switches.
	bool
	writeChannelsA(uint16_t mask)
	{
		// SHDW0/SHDW1 = 0x10/0x11, CMD_A = 0x14
		return writeShadow(0x10, 0x14, mask);
	}

	/// Write the 16-bit channel mask into the bank B shadow registers
	/// and copy them to the bank B switches.
	bool
	writeChannelsB(uint16_t mask)
	{
		// SHDW2/SHDW3 = 0x12/0x13, CMD_B = 0x15
		return writeShadow(0x12, 0x15, mask);
	}

private:
	bool
	writeShadow(uint8_t shadowRegister, uint8_t commandRegister, uint16_t mask)
	{
		// SHDWx: low byte, SHDWx+1: high byte, auto-incrementing register address
		buffer[0] = shadowRegister;
		buffer[1] = uint8_t(mask);
		buffer[2] = uint8_t(mask >> 8);
		if (not this->write(buffer, 3))
			return false;

		// 0b10001 = copy shadow registers of this bank to the switches
		buffer[0] = commandRegister;
		buffer[1] = 0b10001;
		return this->write(buffer, 2);
	}

	uint8_t buffer[3];
};

/**
 * MAX14661 SPI Transport Layer.
 *
 * In SPI mode the MAX14661 has no addressable registers: it is a plain
 * 32-bit shift register and all 32 switches (16 of bank A, 16 of bank B)
 * are transitioned simultaneously on the rising edge of CS. This transport
 * therefore caches both channel masks locally and always shifts out the
 * complete 32-bit word, so that updating one bank does not disturb the
 * other.
 *
 * The SPI interface requires Mode3 and can be clocked with up to ~10MHz.
 *
 * @see Max14661
 *
 * @tparam	Cs	connected chip select pin
 *
 * @ingroup modm_driver_max14661
 * @author	Niklas Hauser
 */
template < class SpiMaster, class Cs >
class Max14661TransportSpi : public modm::SpiDevice< SpiMaster >
{
public:
	Max14661TransportSpi(uint8_t)
	{
		Cs::setOutput(modm::Gpio::High);
	}

protected:
	/// Set the bank A channel mask and shift the complete 32-bit switch
	/// state into the device.
	bool
	writeChannelsA(uint16_t mask)
	{
		maskA = mask;
		return writeShiftRegister();
	}

	/// Set the bank B channel mask and shift the complete 32-bit switch
	/// state into the device.
	bool
	writeChannelsB(uint16_t mask)
	{
		maskB = mask;
		return writeShiftRegister();
	}

private:
	bool
	writeShiftRegister()
	{
		modm::this_fiber::poll([&]{ return this->acquireMaster(); });
		Cs::reset();

		// Table 4: SW16B..SW09B, SW08B..SW01B, SW16A..SW09A, SW08A..SW01A
		const uint8_t buffer[4]
		{
			uint8_t(maskB >> 8),
			uint8_t(maskB),
			uint8_t(maskA >> 8),
			uint8_t(maskA),
		};
		SpiMaster::transfer(buffer, nullptr, 4);

		if (this->releaseMaster())
			Cs::set();

		return true;
	}

	uint16_t maskA{0};
	uint16_t maskB{0};
};

} // namespace modm
