// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__BUFFERED_GRAPHIC_DISPLAY_HPP
	#error	"Don't include this file directly, use 'buffered_graphic_display.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
xpcc::BufferedGraphicDisplay<Width, Height>::clear()
{
	for (uint_fast16_t y = 0; y < Height / 8; ++y) {
		for (uint_fast16_t x = 0; x < Width; ++x) {
			this->display_buffer[x][y] = 0;
		}
	}

	// reset the cursor
	this->cursor = glcd::Point(0, 0);
}

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
xpcc::BufferedGraphicDisplay<Width, Height>::drawHorizontalLine(
		glcd::Point start,
		uint16_t length)
{
	const uint16_t y = start.getY() / 8;

	if (this->foregroundColor == glcd::Color::black())
	{
		const uint8_t mask = 1 << (start.getY() & 0x07);
		for (uint_fast16_t x = start.getX(); x < static_cast<uint16_t>(start.getX() + length); ++x) {
			if( x < Width && y < Height ) {
				this->display_buffer[x][y] |= mask;
			}
		}
	}
	else {
		const uint8_t mask = ~(1 << (start.getY() & 0x07));
		for (uint_fast16_t x = start.getX(); x < static_cast<uint16_t>(start.getX() + length); ++x) {
			if( x < Width && y < Height ) {
				this->display_buffer[x][y] &= mask;
			}
		}
	}
}

// ----------------------------------------------------------------------------
template <uint16_t Width, uint16_t Height>
void
xpcc::BufferedGraphicDisplay<Width, Height>::drawImageRaw(glcd::Point upperLeft,
		uint16_t width, uint16_t height,
		xpcc::accessor::Flash<uint8_t> data)
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
						this->display_buffer[x][y] = data[i + k * width];
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
xpcc::BufferedGraphicDisplay<Width, Height>::setPixel(int16_t x, int16_t y)
{
	if (static_cast<uint16_t>(x) < Width && static_cast<uint16_t>(y) < Height) {
		this->display_buffer[x][y / 8] |= (1 << (y & 0x07));
	}
}

template <uint16_t Width, uint16_t Height>
void
xpcc::BufferedGraphicDisplay<Width, Height>::clearPixel(int16_t x, int16_t y)
{
	if (static_cast<uint16_t>(x) < Width && static_cast<uint16_t>(y) < Height) {
		this->display_buffer[x][y / 8] &= ~(1 << (y & 0x07));
	}
}

template <uint16_t Width, uint16_t Height>
bool
xpcc::BufferedGraphicDisplay<Width, Height>::getPixel(int16_t x, int16_t y)
{
	if (static_cast<uint16_t>(x) < Width && static_cast<uint16_t>(y) < Height) {
		return (this->display_buffer[x][y / 8] & (1 << (y & 0x07)));
	}
	else {
		return false;
	}
}
