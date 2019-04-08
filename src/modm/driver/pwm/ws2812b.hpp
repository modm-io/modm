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

namespace modm
{

/// @ingroup modm_driver_ws2812
template< class SpiMaster, class Output, size_t LEDs >
class Ws2812b
{										  // 7654 3210 7654 3210 7654 3210
	static constexpr uint32_t base_mask  = 0b0010'0100'1001'0010'0100'1001;
	static constexpr uint32_t clear_mask = base_mask << 1;

	static constexpr size_t length = LEDs * 9;
	uint8_t rgb[length + 1]; // +1 for zero byte for reset

	static constexpr uint16_t
	spread(uint8_t nibble)
	{
		return ((nibble & 0b0001) << 10) | ((nibble & 0b0010) << 6) |
			   ((nibble & 0b0100) <<  5) | ((nibble & 0b1000) << 1);
	}

public:
	static constexpr size_t size = LEDs;

	Ws2812b()
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
		size_t ii=0;
		for (;ii < length; ii += 3)
		{
			*reinterpret_cast<uint32_t*>(rgb + ii) = base_mask;
		}
		rgb[length] = 0;
	}

	void
	setColor(size_t index, uint8_t r, uint8_t g, uint8_t b)
	{
		if (index >= LEDs) return;

		const uint8_t color[3] = {g, r, b};
		for (size_t ii = 0; ii < 3; ii++)
		{
			const uint32_t c = (spread(color[ii]) << 12) | spread(color[ii] >> 4);
			uint32_t *const value = reinterpret_cast<uint32_t*>(rgb + index * 9 + ii*3);
			*value = (*value & ~clear_mask) | c;
		}
	}

	void
	write()
	{
		for (const auto data : rgb) {
			while (not SpiMaster::Hal::isTransmitRegisterEmpty()) ;
			SpiMaster::Hal::write(data);
		}
	}
};

}	// namespace modm
