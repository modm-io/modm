/*
 * Copyright (c) 2021, Odin Holmes
 * Copyright (c) 2021, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_MCP7941X_HPP
#define MODM_MCP7941X_HPP

#include <array>
#include <optional>

#include <modm/architecture/interface/register.hpp>
#include <modm/architecture/interface/i2c_device.hpp>
#include <modm/processing/protothread.hpp>

namespace modm
{

/// @ingroup modm_driver_mcp7941x
struct mcp7941x
{
	/// days, months, etc. are decoded (BCD) in this struct
	struct modm_packed
	DateTime
	{
		uint8_t days;
		uint8_t months;
		uint8_t years;
		uint8_t seconds;
		uint8_t minutes;
		uint8_t hours;
	};
};

/**
 * @ingroup modm_driver_mcp7941x
 * @author	Odin Holmes
 * @author	Raphael Lehmann
 */
template < class I2cMaster >
class Mcp7941x :	public mcp7941x,
					public modm::I2cDevice<I2cMaster, 2>
{
public:
	Mcp7941x(uint8_t address = 0x6f);

	modm::ResumableResult<std::optional<modm::mcp7941x::DateTime>>
	getDateTime();

	modm::ResumableResult<bool>
	setDateTime(DateTime);

	modm::ResumableResult<bool>
	oscillatorRunning();

private:
	constexpr uint8_t
	decodeBcd(uint8_t bcd)
	{
		return (bcd / 16 * 10) + (bcd % 16);
	}

	constexpr uint8_t
	encodeBcd(uint8_t decimal)
	{
		return (decimal / 10 * 16) + (decimal % 10);
	}

private:
	DateTime dateTime;

	//address definition for registers of the MCP7941x
	const uint8_t addr_seconds = 0x00;
	const uint8_t addr_minutes = 0x01;
	const uint8_t addr_hours = 0x02;
	const uint8_t addr_weekday = 0x03; //not using day of the week
	const uint8_t addr_days = 0x04;
	const uint8_t addr_months = 0x05;
	const uint8_t addr_years = 0x06;

	uint8_t scratch[8];
};

/**
 * @ingroup modm_driver_mcp7941x
 * @author	Raphael Lehmann
 */
template < class I2cMaster >
class Mcp7941xEeprom :	public modm::I2cDevice<I2cMaster, 2>
{
public:
	Mcp7941xEeprom(uint8_t address = 0x57);

	modm::ResumableResult<std::optional<std::array<uint8_t, 8>>>
	getUniqueId();

private:
	const uint8_t addr_unique_id = 0xF0;

	std::array<uint8_t, 8> data{};
};

} // namespace modm

#include "mcp7941x_impl.hpp"

#endif // MODM_MCP7941X_HPP
