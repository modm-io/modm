/*
 * Copyright (c) 2009-2011, 2013, 2019, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_MONOCHROME_GRAPHIC_DISPLAY_VERTICAL_HPP
#error "Don't include this file directly, use 'monochrome_graphic_display_vertical.hpp' instead!"
#endif

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawHorizontalLine(glcd::Point start,
																		  uint16_t length)
{
	if (start.y < 0 or start.y >= Height) return;
	const uint8_t byte = 1 << (start.y % 8);
	const int16_t end = std::min<int16_t>(start.x + length, Width);
	for (int16_t x = std::max<int16_t>(start.x, 0); x < end; x++)
		this->buffer[start.y / 8][x] |= byte;
}

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawVerticalLine(glcd::Point start,
																		uint16_t length)
{
	if (start.x < 0 or start.x >= Width) return;
	const int16_t end = std::min<int16_t>(start.y + length, Height);
	for (int16_t y = std::max<int16_t>(start.y, 0); y < end; y++)
		this->buffer[y / 8][start.x] |= 1 << (y % 8);
}

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawImageRaw(
	glcd::Point start, uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	if ((start.y % 8) == 0)
	{
		uint16_t row = start.y / 8;
		uint16_t rowCount = (height + 7) / 8;  // always round up

		if ((height % 8) == 0)
		{
			for (uint_fast16_t i = 0; i < width; i++)
			{
				for (uint_fast16_t k = 0; k < rowCount; k++)
				{
					uint16_t x = start.x + i;
					uint16_t y = k + row;

					if (x < Width and y < Height / 8)
					{
						this->buffer[y][x] = data[i + k * width];
					}
				}
			}
			return;
		}
	}

	GraphicDisplay::drawImageRaw(start, width, height, data);
}

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::setPixel(int16_t x, int16_t y)
{
	if (uint16_t(x) < Width and uint16_t(y) < Height) { this->buffer[y / 8][x] |= (1 << y % 8); }
}

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::clearPixel(int16_t x, int16_t y)
{
	if (uint16_t(x) < Width and uint16_t(y) < Height) { this->buffer[y / 8][x] &= ~(1 << y % 8); }
}

template<int16_t Width, int16_t Height>
bool
modm::MonochromeGraphicDisplayVertical<Width, Height>::getPixel(int16_t x, int16_t y) const
{
	if (uint16_t(x) < Width and uint16_t(y) < Height)
	{
		return (this->buffer[y / 8][x] & (1 << y % 8));
	} else
	{
		return false;
	}
}