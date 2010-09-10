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
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#include <stdlib.h>
#include <xpcc/math/utils/bit_operation.hpp>

#include "graphic_display.hpp"

// ----------------------------------------------------------------------------
xpcc::GraphicDisplay::GraphicDisplay() :
	draw(&xpcc::GraphicDisplay::setPixel)
{
}

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::setColor(Color color)
{
	switch (color)
	{
		case BLACK:
			draw = &xpcc::GraphicDisplay::setPixel;
			break;
			
		case WHITE:
			draw = &xpcc::GraphicDisplay::clearPixel;
			break;
	}
	this->color = color;
}

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::drawPixel(uint8_t x, uint8_t y)
{
	(this->*draw)(x, y);
}

void
xpcc::GraphicDisplay::drawLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2)
{
	if (x1 == x2)
	{
		// x1|y1 must be the upper point
		if (y1 > y2) {
			xpcc::swap(y1, y2);
		}
		this->drawVerticalLine(x1, y1, y2 - y1);
	}
	else if (y1 == y2)
	{	
		// x1|y1 must be the left point
		if (x1 > x2) {
			xpcc::swap(x1, x2);
		}
		this->drawHorizontalLine(x1, y1, x2 - x1);
	}
	else
	{
		// bresenham algorithm
		bool steep = abs(y2 - y1) > abs(x2 - x1);
		if (steep) {
			xpcc::swap(x1, y1);
			xpcc::swap(x2, y2);
		}
		if (x1 > x2) {
			xpcc::swap(x1, x2);
			xpcc::swap(y1, y2);
		}
		
		int8_t deltaX = x2 - x1;
		int8_t deltaY = abs(y2 - y1);
		int8_t error = deltaX / 2;
		
		int8_t yStep;
		int8_t y = y1;
		
		if (y1 < y2) {
			yStep = 1;
		}
		else {
			yStep = -1;
		}
		
		for (uint8_t x = x1; x < x2; ++x)
		{
			if (steep) {
				(this->*draw)(y, x);
			}
			else {
				(this->*draw)(x, y);
			}
			error = error - deltaY;
			if (error < 0) {
				y += yStep;
				error += deltaX;
			}
		}
	}
}

void
xpcc::GraphicDisplay::drawHorizontalLine(uint8_t x, uint8_t y, uint8_t length)
{
	for (uint8_t i = x; i < x + length; ++i) {
		(this->*draw)(i, y);
	}
}

void
xpcc::GraphicDisplay::drawVerticalLine(uint8_t x, uint8_t y, uint8_t length)
{
	for (uint8_t i = y; i < y + length; ++i) {
		(this->*draw)(x, i);
	}
}

void
xpcc::GraphicDisplay::drawRectangle(uint8_t x, uint8_t y,
		uint8_t width, uint8_t height)
{
	uint8_t x2 = x + width - 1;
	uint8_t y2 = y + height - 1;
	
	this->drawHorizontalLine(x, y,  width);
	this->drawHorizontalLine(x, y2, width);
	this->drawVerticalLine(x,  y, height);
	this->drawVerticalLine(x2, y, height);
}

void
xpcc::GraphicDisplay::drawRoundedRectangle(uint8_t x, uint8_t y,
		uint8_t width, uint8_t height, uint8_t radius)
{
	if (radius == 0) {
		this->drawRectangle(x, y, width, height);
	}
	
	int8_t x1 = 0;
	int8_t y1 = radius;
  	int16_t f = 3 - 2 * radius;
	
	while (x1 <= y1)
	{
		(this->*draw)(x + radius - x1, y + radius - y1);
		(this->*draw)(x + radius - x1, y + height - radius + y1);
		(this->*draw)(x + radius - y1, y + radius - x1);
		(this->*draw)(x + radius - y1, y + height - radius + x1);
		
		(this->*draw)(x + width - radius + x1, y + radius - y1);
		(this->*draw)(x + width - radius + x1, y + height - radius + y1);
		(this->*draw)(x + width - radius + y1, y + radius - x1);
		(this->*draw)(x + width - radius + y1, y + height - radius + x1);
		
		if (f < 0) {
			f += (4 * x1 + 6);
		}
		else {
			f += (4 * (x1 - y1) + 10);
			y1--;
		}
		x1++;
	}
	
	this->drawHorizontalLine(x + radius, y, width - (2 * radius));
	this->drawHorizontalLine(x + radius, y + height, width - (2 * radius));
	this->drawVerticalLine(x, y + radius, height - (2 * radius));
	this->drawVerticalLine(x + width, y + radius, height - (2 * radius));
}

void
xpcc::GraphicDisplay::drawCircle(uint8_t cx, uint8_t cy, uint8_t radius)
{
	if (radius == 0) {
		return;
	}
	
	int8_t error = -radius;
	int8_t x = radius;
	int8_t y = 0;
	
	while (x > y)
	{
		this->drawCircle4(cx, cy, x, y);
		this->drawCircle4(cx, cy, y, x);
		
		error += y;
		++y;
		error += y;
		
		if (error >= 0)
		{
			--x;
			error -= x;
			error -= x;
		}
	}
	this->drawCircle4(cx, cy, x, y);
}

void
xpcc::GraphicDisplay::drawCircle4(uint8_t cx, uint8_t cy, uint8_t x, uint8_t y)
{
	(this->*draw)(cx + x, cy + y);
	(this->*draw)(cx - x, cy - y);
	if (x != 0) { (this->*draw)(cx - x, cy + y); }
	if (y != 0) { (this->*draw)(cx + x, cy - y); }
}

void
xpcc::GraphicDisplay::drawEllipse(uint8_t cx, uint8_t cy, uint8_t rx, uint8_t ry)
{
	int32_t rx_2 = rx * rx;
	int32_t ry_2 = ry * ry;
	
	int16_t x = 0;
	int16_t y = ry;
	
	int32_t fx = 0;
	int32_t fy = rx_2 * 2 * ry;
	
	int32_t p = ry_2 - (rx_2 * ry) + (rx_2 + 2) / 4;
	
	drawCircle4(cx, cy, x, y);
	while (fx < fy)
	{
		x++;
		fx += ry_2 * 2;
		
		if (p < 0) {
			p += (fx + ry_2);
		}
		else {
			y--;
			fy -= rx_2 * 2;
			p += (fx + ry_2 - fy);
		}
		
		drawCircle4(cx, cy, x, y);
	}
	
	p = ((ry_2 * (4 * x * x + 4 * x + 1) / 2) + 
		 2 * (rx_2 * (y - 1) * (y - 1)) -
		 2 * (rx_2 * ry_2)
		 + 1) / 2;
	
	while (y > 0) {
		y--;
		fy -= rx_2 * 2;
		
		if (p >= 0) {
			p += (rx_2 - fy);
		}
		else {
			x++;
			fx += ry_2 * 2;
			p += (fx + rx_2 - fy);
		}
		
		drawCircle4(cx, cy, x, y);
	}
}

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::drawImage(uint8_t x, uint8_t y,
		xpcc::accessor::Flash<uint8_t> image)
{
	uint8_t width = image[0];
	uint8_t height = image[1];
	uint8_t rows = (height + 7) / 8;
	
	for (uint8_t i = 0; i < width; i++)
	{
		for (uint8_t k = 0; k < rows; k++)
		{
			uint8_t byte = image[2 + i + k * width];
			for (uint8_t j = 0; j < 8; j++)
			{
				if (byte & 0x01) {
					this->setPixel(x + i, y + k * 8 + j);
				}
				else {
					this->clearPixel(x + i, y + k * 8 + j);
				}
				byte >>= 1;
			}
		}
	}
}

// ----------------------------------------------------------------------------
void
xpcc::GraphicDisplay::fillRectangle(uint8_t x, uint8_t y,
		uint8_t width, uint8_t height)
{
	for (uint8_t i = x; (i < x + width) && (i < 128); i++) {
		for (uint8_t k = y; (k < y + height) && (k < 64); k++) {
			(this->*draw)(i, k);
		}
	}
}
