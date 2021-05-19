/*
 * Copyright (c) 2009-2010, 2012, Fabian Greif
 * Copyright (c) 2010, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, David Hebbeker
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_COLOR_RGB_HPP
#error "Don't include this file directly, use 'rgb.hpp' instead!"
#endif

#include <algorithm>

namespace modm::color
{

// TODO Finish generalisation for uint16_t
template<std::unsigned_integral T>
template<std::unsigned_integral U>
constexpr RgbT<T>::RgbT(const HsvT<U> &hsv)
{
	uint16_t vs = hsv.value * hsv.saturation;
	uint16_t h6 = 6 * hsv.hue;

	T p = ((hsv.value << 8) - vs) >> 8;
	T i = h6 >> 8;
	uint16_t f = ((i | 1) << 8) - h6;
	if (i & 1) { f = -f; }
	T u = (((uint32_t)hsv.value << 16) - (uint32_t)vs * f) >> 16;

	switch (i)
	{
		case 0: red = hsv.value; green = u; blue = p; break;
		case 1: red = u; green = hsv.value; blue = p; break;
		case 2: red = p; green = hsv.value; blue = u; break;
		case 3: red = p; green = u; blue = hsv.value; break;
		case 4: red = u; green = p; blue = hsv.value; break;
		case 5: red = hsv.value; green = p; blue = u; break;
	}
}

}	// namespace modm::color
