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

#include <span>
#include <type_traits>

namespace modm::detail::st7735
{

template<typename E>
concept Enum8 = requires(E) { std::is_same_v<std::underlying_type_t<E>, uint8_t>; };

}  // namespace modm::detail::st7735

namespace modm
{

/// @ingroup modm_driver_st7735
template<typename SPI, typename CS, typename RST, typename DC>
struct St7735SPIInterface
{
	using Spi = SPI;
	using Cs = CS;
	using Reset = RST;
	using DataCommands = DC;
	using data_t = std::span<const uint8_t>;

	static void
	initialize()
	{
		Reset::setOutput(true);
		Cs::setOutput(true);
		DataCommands::setOutput();
	}

	static void
	beginCommand(uint8_t command)
	{
		DataCommands::reset();
		Cs::reset();
		Spi::transfer(command);
	}

	template<detail::st7735::Enum8 Command>
	static void
	beginCommand(Command command)
	{ beginCommand(static_cast<uint8_t>(command)); }

	static void
	sendCommand(uint8_t command)
	{
		beginCommand(command);
		end();
	}

	template<detail::st7735::Enum8 Command>
	static void
	sendCommand(Command command)
	{ sendCommand(static_cast<uint8_t>(command)); }

	template<typename Data>
	static void
	sendCommand(uint8_t command, const Data &data)
	{
		beginCommand(command);
		sendData(data);
	}

	template<detail::st7735::Enum8 Command, typename Data>
	static void
	sendCommand(Command command, const Data &data)
	{ sendCommand(static_cast<uint8_t>(command), data); }

	static void
	switchToDataMode()
	{ DataCommands::set(); }

	static void
	sendData(uint8_t data)
	{
		switchToDataMode();
		Spi::transfer(data);
		end();
	}

	static void
	sendData(data_t data)
	{
		switchToDataMode();
		Spi::transfer(data.data(), nullptr, data.size());
		end();
	}

	template<typename Data>
	static void
	sendData(const Data &data)
	{
		switchToDataMode();
		Spi::transfer(reinterpret_cast<const uint8_t *>(&data), nullptr, sizeof(data));
		end();
	}

	static void
	continueData(uint8_t data)
	{ Spi::transfer(data); }

	static void
	continueData(data_t data)
	{ Spi::transfer(data.data(), nullptr, data.size()); }

	static void
	end()
	{ Cs::set(); }
};

}  // namespace modm
