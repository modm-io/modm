/*
 * Copyright (c) 2019, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include <modm/math/units.hpp>
#include <modm/architecture/interface/spi_master.hpp>
#include <modm/ui/color.hpp>

namespace modm
{

/// @ingroup modm_driver_apa102
template< class SpiMaster, size_t LEDs >
class Apa102
{
protected:
	static constexpr size_t length = 4 + 4*LEDs + ((LEDs+15)/16);
	uint8_t data[length] modm_aligned(4);
public:
	static constexpr size_t size = LEDs;

	Apa102() : data{0,0,0,0}
	{
		clear();
		// using 0x00 as flushing bits makes this compatible with sk9822
		for (size_t ii=4 + 4*LEDs; ii < length; ii++)
			data[ii] = 0x00;
	}

	void
	clear()
	{
		for (size_t ii=0; ii < LEDs; ii++)
			reinterpret_cast<uint32_t*>(data)[1+ii] = 0xff;
	}

	bool
	setColorBrightness(size_t index, const color::Rgb &color, uint8_t brightness)
	{
		if (index >= LEDs) return false;
		uint32_t value = (color.red << 24) | (color.green << 16) |
						 (color.blue << 8) | (brightness | 0xe0);
		reinterpret_cast<uint32_t*>(data)[1+index] = value;
		return true;
	}

	bool
	setColor(size_t index, const color::Rgb &color)
	{
		if (index >= LEDs) return false;
		// read the brightness value and clear all colors
		uint32_t value = reinterpret_cast<const uint32_t*>(data)[1+index] & 0xfful;
		// set all colors
		value |= (color.red << 24) | (color.green << 16) | (color.blue << 8);
		// write back entire value
		reinterpret_cast<uint32_t*>(data)[1+index] = value;
		return true;
	}

	color::Rgb
	getColor(size_t index) const
	{
		if (index >= LEDs) return {};
		const uint32_t value = reinterpret_cast<const uint32_t*>(data)[1+index];
		return {uint8_t(value >> 24), uint8_t(value >> 16), uint8_t(value >> 8)};
	}

	void
	setBrightness(size_t index, uint8_t brightness)
	{
		if (index >= LEDs) return;
		data[4 + index*4] = brightness | 0xe0;
	}

	uint8_t
	getBrightness(size_t index) const
	{
		if (index >= LEDs) return 0;
		return data[4 + index*4] & ~0xe0;
	}

	modm::ResumableResult<void>
	write()
	{
		return SpiMaster::transfer(data, nullptr, length);
	}
};

}	// namespace modm
