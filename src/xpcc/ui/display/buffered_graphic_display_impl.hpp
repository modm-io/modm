/*
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2016, Antal Szabo
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_BUFFERED_GRAPHIC_DISPLAY_HPP
	#error	"Don't include this file directly, use 'buffered_graphic_display.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
modm::BufferedGraphicDisplay<Width, Height>::clear()
{
	for (uint_fast16_t y = 0; y < Height / 8; ++y) {
		for (uint_fast16_t x = 0; x < Width; ++x) {
			this->buffer[x][y] = 0;
		}
	}

	// reset the cursor
	this->cursor = glcd::Point(0, 0);
}

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
modm::BufferedGraphicDisplay<Width, Height>::drawHorizontalLine(
		glcd::Point start,
		uint16_t length)
{
	const uint16_t y = start.getY() / 8;

	if (this->foregroundColor == glcd::Color::black())
	{
		const uint8_t mask = 1 << (start.getY() & 0x07);
		for (uint_fast16_t x = start.getX(); x < static_cast<uint16_t>(start.getX() + length); ++x) {
			if( x < Width && y < Height ) {
				this->buffer[x][y] |= mask;
			}
		}
	}
	else {
		const uint8_t mask = ~(1 << (start.getY() & 0x07));
		for (uint_fast16_t x = start.getX(); x < static_cast<uint16_t>(start.getX() + length); ++x) {
			if( x < Width && y < Height ) {
				this->buffer[x][y] &= mask;
			}
		}
	}
}

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
modm::BufferedGraphicDisplay<Width, Height>::drawImageRaw(glcd::Point upperLeft,
		uint16_t width, uint16_t height,
		modm::accessor::Flash<uint8_t> data)
{
	if ((upperLeft.getY() & 0x07) == 0)
	{
		uint16_t row = upperLeft.getY() / 8;
		uint16_t rowCount = (height + 7) / 8;	// always round up

		if ((height & 0x07) == 0)
		{
			for (uint_fast16_t i = 0; i < width; i++)
			{
				for (uint_fast16_t k = 0; k < rowCount; k++)
				{
					uint16_t x = upperLeft.getX() + i;
					uint16_t y = k + row;

					if( x < Width && y < Height ) {
						this->buffer[x][y] = data[i + k * width];
					}
				}
			}
			return;
		}
	}

	GraphicDisplay::drawImageRaw(upperLeft, width, height, data);
}

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
modm::BufferedGraphicDisplay<Width, Height>::setPixel(int16_t x, int16_t y)
{
	if (static_cast<uint16_t>(x) < Width && static_cast<uint16_t>(y) < Height) {
		this->buffer[x][y / 8] |= (1 << (y & 0x07));
	}
}

template <uint16_t Width, uint16_t Height>
void
modm::BufferedGraphicDisplay<Width, Height>::clearPixel(int16_t x, int16_t y)
{
	if (static_cast<uint16_t>(x) < Width && static_cast<uint16_t>(y) < Height) {
		this->buffer[x][y / 8] &= ~(1 << (y & 0x07));
	}
}

template <uint16_t Width, uint16_t Height>
bool
modm::BufferedGraphicDisplay<Width, Height>::getPixel(int16_t x, int16_t y)
{
	if (static_cast<uint16_t>(x) < Width && static_cast<uint16_t>(y) < Height) {
		return (this->buffer[x][y / 8] & (1 << (y & 0x07)));
	}
	else {
		return false;
	}
}
