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
#include <modm/architecture/interface/unaligned.hpp>
#include <modm/ui/color.hpp>

namespace modm
{

/// @ingroup modm_driver_sk6812
template< class SpiMaster, class Output, size_t LEDs >
class Sk6812w
{
protected:								  // 7654 3210 7654 3210 7654 3210
	static constexpr uint32_t base_mask  = 0b0010'0100'1001'0010'0100'1001;
	static constexpr uint32_t clear_mask = base_mask << 1;

	static constexpr size_t length = LEDs * 12;
	uint8_t data[length + 1]; // +1 for zero byte for reset

	static constexpr uint16_t
	spread(uint8_t nibble)
	{
		return ((nibble & 0b0001) << 10) | ((nibble & 0b0010) << 6) |
			   ((nibble & 0b0100) <<  5) | ((nibble & 0b1000) << 1);
	}

	static constexpr uint8_t
	gather(uint32_t pattern)
	{
		return ((pattern >> 10) & 0b0001) | ((pattern >> 6) & 0b0010) |
			   ((pattern >> 5) & 0b0100) | ((pattern >> 1) & 0b1000);
	}

public:
	static constexpr size_t size = LEDs;

	Sk6812w()
	{
		clear();
	}

	template< class SystemClock >
	void
	initialize()
	{
		SpiMaster::template connect<Output::template Mosi>();
		SpiMaster::template initialize<SystemClock, MHz(3), pct(10)>();
		SpiMaster::setDataOrder(SpiMaster::DataOrder::LsbFirst);
		SpiMaster::Hal::write(uint8_t(0));
	}

	void
	clear()
	{
		for (size_t ii=0; ii < length; ii += 3)
		{
			*modm::asUnaligned<uint32_t*>(data + ii) = base_mask;
		}
		data[length] = 0;
	}

	void
	setColorBrightness(size_t index, const color::Rgb &color, uint8_t brightness)
	{
		if (index >= LEDs) return;

		const uint8_t colors[] = {color.green, color.red, color.blue, brightness};
		for (size_t ii = 0; ii < 4; ii++)
		{
			const uint32_t c = (spread(colors[ii]) << 12) | spread(colors[ii] >> 4);
			auto *value = modm::asUnaligned<uint32_t*>(data + index * 12 + ii*3);
			*value = (uint32_t(*value) & ~clear_mask) | c;
		}
	}

	void
	setColor(size_t index, const color::Rgb &color)
	{
		if (index >= LEDs) return;

		const uint8_t colors[] = {color.green, color.red, color.blue};
		for (size_t ii = 0; ii < 3; ii++)
		{
			const uint32_t c = (spread(colors[ii]) << 12) | spread(colors[ii] >> 4);
			auto *value = modm::asUnaligned<uint32_t*>(data + index * 12 + ii*3);
			*value = (uint32_t(*value) & ~clear_mask) | c;
		}
	}

	color::Rgb
	getColor(size_t index) const
	{
		if (index >= LEDs) return {};

		uint8_t color[3];
		for (size_t ii = 0; ii < 3; ii++)
		{
			const auto value = *modm::asUnaligned<const uint32_t*>(data + index * 12 + ii*3);
			const uint8_t c = (gather(value) << 4) | gather(value >> 12);
			color[ii] = c;
		}
		return {color[1], color[0], color[2]};
	}

	void
	setBrightness(size_t index, uint8_t brightness)
	{
		if (index >= LEDs) return;

		const uint32_t c = (spread(brightness) << 12) | spread(brightness >> 4);
		auto *value = modm::asUnaligned<uint32_t*>(data + index * 12 + 3*3);
		*value = (uint32_t(*value) & ~clear_mask) | c;
	}

	uint8_t
	getBrightness(size_t index) const
	{
		if (index >= LEDs) return {};

		const auto value = *modm::asUnaligned<const uint32_t*>(data + index * 12 + 3*3);
		return (gather(value) << 4) | gather(value >> 12);
	}

	void
	write()
	{
		for (const auto value : data) {
			while (not SpiMaster::Hal::isTransmitRegisterEmpty()) ;
			SpiMaster::Hal::write(value);
		}
	}
};

}	// namespace modm
