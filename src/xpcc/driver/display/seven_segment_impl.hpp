/*
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2013-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_SEVEN_SEGMENT_DISPLAY_HPP
#	error "Don't include this file directly, use 'seven_segment.hpp' instead!"
#endif

// ----------------------------------------------------------------------------

template<typename Spi, typename Load, uint8_t DIGITS>
void
modm::sevenSegment::SevenSegmentDisplay<Spi, Load, DIGITS>::initialize()
{

}

template<typename Spi, typename Load, uint8_t DIGITS>
void
modm::sevenSegment::SevenSegmentDisplay<Spi, Load, DIGITS>::write(uint32_t val)
{
	for (uint_fast8_t ii = 0; ii < DIGITS; ++ii)
	{
		Spi::write(segmentLutHex[val % 10]);
		val /= 10;
	}

	// on rising edge data is moved to storage registers
	Load::reset();
	Load::set();

}

template<typename Spi, typename Load, uint8_t DIGITS>
void
modm::sevenSegment::SevenSegmentDisplay<Spi, Load, DIGITS>::writeHex(uint32_t val)
{
	for (uint_fast8_t ii = 0; ii < DIGITS; ++ii)
	{
		Spi::write(segmentLutHex[val & 0x0f]);
		val >>= 4;
	}

	// on rising edge data is moved to storage registers
	Load::reset();
	Load::set();
}
