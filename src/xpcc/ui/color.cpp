// coding: utf-8
/* Copyright (c) 2012, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#include "color.hpp"

// ----------------------------------------------------------------------------
namespace xpcc {
namespace color {
template<> template<>
void
HsvT<uint8_t>::toRgb(RgbT<uint8_t>* color) const
{
	uint16_t vs = value * saturation;
	uint16_t h6 = 6 * hue;

	uint8_t p = ((value << 8) - vs) >> 8;
	uint8_t i = h6 >> 8;
	uint16_t f = ((i | 1) << 8) - h6;
	if (i & 1) {
		f = -f;
	}

	uint8_t u = (((uint32_t) value << 16) - (uint32_t) vs * f) >> 16;
	uint8_t r = value;
	uint8_t g = value;
	uint8_t b = value;
	switch(i)
	{
		case 0: g = u; b = p; break;
		case 1: r = u; b = p; break;
		case 2: r = p; b = u; break;
		case 3: r = p; g = u; break;
		case 4: r = u; g = p; break;
		case 5: g = p; b = u; break;
	}

	color->red = r;
	color->green = g;
	color->blue = b;
}
} // namespace color
} // namespace xpcc
