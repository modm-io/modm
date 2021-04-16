/*
 * Copyright (c) 2019, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_HORIZONTAL_HPP
#error "Don't include this file directly, use 'monochrome_graphic_display_horizontal.hpp' instead!"
#endif

namespace modm
{
template<int16_t Width, int16_t Height>
void
MonochromeGraphicDisplayHorizontal<Width, Height>::setPixel(int16_t x, int16_t y)
{
	if ((x < Width) and (y < Height)) { buffer[y][x / 8] |= (1 << (x % 8)); }
}

template<int16_t Width, int16_t Height>
void
MonochromeGraphicDisplayHorizontal<Width, Height>::clearPixel(int16_t x, int16_t y)
{
	if ((x < Width) and (y < Height)) { buffer[y][x / 8] &= ~(1 << (x % 8)); }
}

template<int16_t Width, int16_t Height>
bool
MonochromeGraphicDisplayHorizontal<Width, Height>::getPixel(int16_t x, int16_t y)
{
	if ((x < Width) and (y < Height))
		return (buffer[y][x / 8] & (1 << (x % 8)));
	else
		return false;
}
}  // namespace modm
