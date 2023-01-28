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

#include <modm/ui/display/orientation.hpp>
#include <span>

#include "st7789_protocol.hpp"

namespace modm
{

namespace detail::st7789
{
enum ResolutionLimits : uint16_t
{
	MaxWidth = 240,
	MaxHeight = 320,
};
}

/// @ingroup modm_driver_st7789
template<typename Interface, uint16_t Width = 240, uint16_t Height = 320>
	requires(Width <= detail::st7789::MaxWidth && Height <= detail::st7789::MaxHeight)
class St7789Driver

{
public:
	void
	initialize();

	void
	clear(uint16_t color);

	void
	setClipping(uint16_t x, uint16_t y, uint16_t width, uint16_t height);

	using Orientation = glcd::Orientation;
	void setOrientation(Orientation);
	Orientation
	getOrientation() const;

public:
	using Command = detail::st7789::Command;

	void sendCommand(Command);

	template<typename Data>
	void
	sendCommand(Command, Data &&);

	using data = Interface::data_t;

	enum class ByteOrder : uint8_t
	{
		Swap2Bytes,
		Passthrough,
	};

	template<ByteOrder OrderOfBytes = ByteOrder::Swap2Bytes>
	void writeData(data);

public:
	void
	hardReset();

	void
	softReset();

	void
	turnOn();
	void
	turnOff();

	void
	setIdle(bool);

	void
	setSleep(bool);

	void
	setInversion(bool);

	using RgbBgrOrder = detail::st7789::MemoryDataAccessControl::RgbBgrOrder;
	void setRgbBgrOrder(RgbBgrOrder);

protected:
	Orientation orientation_{Orientation::Landscape0};

	using MemoryDataAccessControl = detail::st7789::MemoryDataAccessControl;
	MemoryDataAccessControl madctl_{};

	uint16_t offset_x_{};
	uint16_t offset_y_{};
};

}  // namespace modm

#include "st7789_driver_impl.hpp"
