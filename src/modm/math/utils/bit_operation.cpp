/*
 * Copyright (c) 2009, Georgi Grinshpun
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012-2014, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "bit_operation.hpp"

// ----------------------------------------------------------------------------
std::size_t
modm::bitCount(uint8_t n)
{
	n = ((uint8_t) (n >> 1) & 0x55) + (n & 0x55);
	n = ((uint8_t) (n >> 2) & 0x33) + (n & 0x33);
	n = ((uint8_t) (n >> 4) + n) & 0xf;

	return n;
}

std::size_t
modm::bitCount(uint16_t n)
{
	return (bitCount((uint8_t) (n)) +
			bitCount((uint8_t) (n >> 8)));
}

std::size_t
modm::bitCount(uint32_t n)
{
	n = ((n >> 1) & 0x55555555) + (n & 0x55555555);
	n = ((n >> 2) & 0x33333333) + (n & 0x33333333);
	n = ((n >> 4) & 0x0f0f0f0f) + (n & 0x0f0f0f0f);

	return n % 255;
}
