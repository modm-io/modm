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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>

#include "buffered_graphic_display.hpp"

// ----------------------------------------------------------------------------
void
xpcc::BufferedGraphicDisplay::clear()
{
	for (uint8_t i = 0; i < 8; ++i) {
		for (uint8_t k = 0; k < 128; ++k) {
			this->buffer[k][i] = 0;
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedGraphicDisplay::drawImage(uint8_t x, uint8_t y,
		xpcc::accessor::Flash<uint8_t> image)
{
	div_t row = div(y, 8);
	if (row.rem == 0)
	{
		uint8_t width = image[0];
		uint8_t height = image[1];
		
		uint8_t rows = (height + 7) / 8;
		
		if (rows == height / 8)
		{
			for (uint8_t i = 0; i < width; i++)
			{
				for (uint8_t k = 0; k < rows; k++)
				{
					this->buffer[x + i][k + row.quot] = image[2 + i + k * width];
				}
			}
			return;
		}
	}
	
	GraphicDisplay::drawImage(x, y, image);
}

// ----------------------------------------------------------------------------
void
xpcc::BufferedGraphicDisplay::setPixel(uint8_t x, uint8_t y)
{
	this->buffer[x][y / 8] |= (1 << (y & 0x07));
}

void
xpcc::BufferedGraphicDisplay::clearPixel(uint8_t x, uint8_t y)
{
	this->buffer[x][y / 8] &= ~(1 << (y & 0x07));
}
