// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above copyright
 *	   notice, this list of conditions and the following disclaimer in the
 *	   documentation and/or other materials provided with the distribution.
 *	 * Neither the name of the Roboterclub Aachen e.V. nor the
 *	   names of its contributors may be used to endorse or promote products
 *	   derived from this software without specific prior written permission.
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

#include "graphic_display.hpp"

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::fillRectangle(glcd::Point upperLeft,
		uint16_t width, uint16_t height)
{
	uint16_t i = upperLeft.getX();
	if (i < 0) {
		i = 0;
	}
	
	uint16_t k = upperLeft.getY();
	if (k < 0) {
		k = 0;
	}
	
	for (; (i < upperLeft.getX() + width) && (i < getHeight()); ++i) {
		for (; (k < upperLeft.getY() + height) && (k < getWidth()); ++k) {
			(this->*draw)(i, k);
		}
	}
}

void
xpcc::GraphicDisplay::fillCircle(glcd::Point center, uint16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;
	
	this->drawVerticalLine(glcd::Point(center.getX(), center.getY() - radius), 2 * radius);
	
	while(x < y)
	{
		if (f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x + 1;
		
		this->drawVerticalLine(glcd::Point(center.getX() + x, center.getY() - y), 2 * y);
		this->drawVerticalLine(glcd::Point(center.getX() + y, center.getY() - x), 2 * x);
		this->drawVerticalLine(glcd::Point(center.getX() - x, center.getY() - y), 2 * y);
		this->drawVerticalLine(glcd::Point(center.getX() - y, center.getY() - x), 2 * x);
	}
}
