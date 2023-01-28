/*
 * Copyright (c) 2022, Nikolay Semenov
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

namespace modm
{

template<typename E>
concept Enum8 = requires(E) { std::is_same_v<std::underlying_type_t<E>, uint8_t>; };

/// @ingroup modm_driver_st7789
template<typename SPI, typename CS, typename RST, typename DC>
struct St7789SPIInterface
{
	using Spi = SPI;
	using Cs = CS;
	using Reset = RST;
	using DataCommands = DC;

	//--
	static void
	initialize()
	{
		Reset::setOutput(false);
		Cs::setOutput(true);
		DataCommands::setOutput();
	}

	//--
	static void
	beginCommand(uint8_t command)
	{
		DataCommands::reset();
		Cs::reset();
		Spi::transferBlocking(command);
	}

	template<Enum8 Command>
	static void
	beginCommand(Command command)
	{
		beginCommand(static_cast<uint8_t>(command));
	}

	//--
	static void
	sendCommand(uint8_t command)
	{
		beginCommand(command);
		end();
	}

	template<Enum8 Command>
	static void
	sendCommand(Command command)
	{
		sendCommand(static_cast<uint8_t>(command));
	}

	//--
	template<typename Data>
	static void
	sendCommand(uint8_t command, Data &&data)
	{
		beginCommand(command);
		sendData(std::forward<Data>(data));
	}

	template<Enum8 Command, typename Data>
	static void
	sendCommand(Command command, Data &&data)
	{
		beginCommand(static_cast<uint8_t>(command));
		sendData(std::forward<Data>(data));
	}

	//--
	using data_t = std::span<const uint8_t>;

	//--
	static void
	switchToDataMode()
	{
		DataCommands::set();
	}

	//--
	static void
	sendData(uint8_t data)
	{
		switchToDataMode();
		Spi::transferBlocking(data);
		end();
	}

	static void
	sendData(data_t data)
	{
		switchToDataMode();
		Spi::transferBlocking(data.data(), nullptr, data.size());
		end();
	}

	template<typename Data>
	static void
	sendData(const Data &data)
	{
		switchToDataMode();
		Spi::transferBlocking(reinterpret_cast<const uint8_t *>(&data), nullptr, sizeof(data));
		end();
	}

	//--
	static void
	continueData(uint8_t data)
	{
		Spi::transferBlocking(data);
	}

	static void
	continueData(data_t data)
	{
		Spi::transferBlocking(data.data(), nullptr, data.size());
	}

	template<typename Data>
	static void
	continueData(const Data &data)
	{
		Spi::transferBlocking(reinterpret_cast<const uint8_t *>(&data), nullptr, sizeof(data));
	}

	//--
	static void
	end()
	{
		Cs::set();
	}
};

}  // namespace modm

#include "st7789_driver_impl.hpp"
