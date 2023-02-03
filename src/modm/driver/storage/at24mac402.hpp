/*
 * Copyright (c) 2023, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_AT24MAC402_HPP
#define MODM_AT24MAC402_HPP

#include <modm/driver/storage/i2c_eeprom.hpp>
#include <span>

namespace modm
{

/**
 * AT24MAC402 I2C EEPROM
 *
 * 2 kBit EEPROM with pre-programmed 48 bit MAC address, 128 bit unique id and
 * 8 bit address pointer.
 * The device address can be configured from 0x50 to 0x57 with 3 address input
 * lines.
 *
 * @ingroup	modm_driver_at24mac402
 * @author	Christopher Durand
 */
template <typename I2cMaster>
class At24Mac402 : protected I2cEeprom<I2cMaster, 1>
{
	using Base = I2cEeprom<I2cMaster, 1>;
	static constexpr uint8_t UuidAddress{0x80};
	static constexpr uint8_t MacAddress{0x9A};
public:
	using Base::attachConfigurationHandler;

	/// @param address i2c data base address (0b1010xxx)
	At24Mac402(uint8_t address = 0x50);

	/**
	 * Set I2C data base address (0b1010xxx).
	 * The upper 4 bits will always be forced to 0b1010.
	 */
	void
	setAddress(uint8_t address);

	// Read 48 bit pre-programmed MAC
	modm::ResumableResult<bool>
	readMac(std::span<uint8_t, 6> data);

	// Read 128 bit pre-programmed unique id
	modm::ResumableResult<bool>
	readUniqueId(std::span<uint8_t, 16> data);

	// start documentation inherited
	modm::ResumableResult<bool>
	ping();

	modm::ResumableResult<bool>
	writeByte(uint32_t address, uint8_t data);

	modm::ResumableResult<bool>
	write(uint32_t address, const uint8_t* data, std::size_t length);

	template<typename T>
	modm::ResumableResult<bool>
	write(uint32_t address, const T& data);

	modm::ResumableResult<bool>
	readByte(uint32_t address, uint8_t& data);

	modm::ResumableResult<bool>
	read(uint32_t address, uint8_t* data, std::size_t length);

	template<typename T>
	modm::ResumableResult<bool>
	read(uint32_t address, T& data);
	// end documentation inherited
private:
	uint8_t address_{};
};

}	// namespace modm

#include "at24mac402_impl.hpp"

#endif // MODM_AT24MAC402_HPP
