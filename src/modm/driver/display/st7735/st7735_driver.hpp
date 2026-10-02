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

#include "st7735_protocol.hpp"

namespace modm
{

/// @ingroup modm_driver_st7735
template<typename Interface>
class St7735Driver
{
public:
	explicit constexpr St7735Driver(const st7735::PanelProfile &profile)
		: profile_(profile), bgr_(profile.bgr)
	{}

	void
	initialize();

	void
	clear(uint16_t color);
	void
	setClipping(uint16_t x, uint16_t y, uint16_t width, uint16_t height);
	void
	writeColor(uint16_t color);

	using Orientation = glcd::Orientation;
	void
	setOrientation(Orientation orientation);
	Orientation
	getOrientation() const;

	uint16_t
	getWidth() const;
	uint16_t
	getHeight() const;

	using Command = detail::st7735::Command;
	using data = typename Interface::data_t;

	void
	sendCommand(Command command);

	template<typename Data>
	void
	sendCommand(Command command, const Data &data);

	enum class ByteOrder : uint8_t
	{
		Swap2Bytes,
		Passthrough,
	};

	template<ByteOrder OrderOfBytes = ByteOrder::Swap2Bytes>
	void
	writeData(data data);

	/// Reset a panel with an independently wired reset pin.
	/// The WeAct profile uses software reset because its reset pin is shared with NRST.
	void
	hardReset();
	void
	softReset();
	void
	turnOn();
	void
	turnOff();
	void
	setInversion(bool inversion);
	void
	setRgbBgrOrder(bool bgr);

protected:
	const st7735::PanelProfile &profile_;
	Orientation orientation_{Orientation::Landscape0};
	bool bgr_;
};

}  // namespace modm

#include "st7735_driver_impl.hpp"
