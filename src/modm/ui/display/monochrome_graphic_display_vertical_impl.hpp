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
	if (start.y >= 0 and start.y < Height)
	{
		const int16_t y = start.y / 8;

		const uint8_t byte = 1 << (start.y % 8);
		for (int_fast16_t x = start.x; x < static_cast<int16_t>(start.x + length); ++x)
		{
			if (x < Width) { this->buffer[y][x] |= byte; }
		}
	}
}

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::drawVerticalLine(glcd::Point start,
																		uint16_t length)
{
	if (start.x >= 0 and start.x < Width)
	{
		const int8_t end_y = start.y + length;
		const uint8_t y_last = end_y / 8;

		uint_fast8_t y = start.y / 8;
		// Mask out start
		uint_fast8_t byte = 0xFF << start.y % 8;
		while (y != y_last)
		{
			if (y < Height / 8)
			{
				this->buffer[y][start.x] |= byte;
				byte = 0xFF;
			}
			y++;
		}
		// Mask out end
		if (y < Height / 8)
		{
			byte &= 0xFF >> (8 - end_y % 8);
			this->buffer[y][start.x] |= byte;
		}
	}
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

					if (x < Width and y < Height)
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
	if (x < Width and y < Height) { this->buffer[y / 8][x] |= (1 << y % 8); }
}

template<int16_t Width, int16_t Height>
void
modm::MonochromeGraphicDisplayVertical<Width, Height>::clearPixel(int16_t x, int16_t y)
{
	if (x < Width and y < Height) { this->buffer[y / 8][x] &= ~(1 << y % 8); }
}

template<int16_t Width, int16_t Height>
bool
modm::MonochromeGraphicDisplayVertical<Width, Height>::getPixel(int16_t x, int16_t y) const
{
	if (x < Width and y < Height)
	{
		return (this->buffer[y / 8][x] & (1 << y % 8));
	} else
	{
		return false;
	}
}