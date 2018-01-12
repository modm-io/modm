/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2014, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2016, Antal Szabó
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <cstdlib>
#include <modm/math/utils/bit_operation.hpp>

#include "font/fixed_width_5x8.hpp"
#include "graphic_display.hpp"

// ----------------------------------------------------------------------------
modm::GraphicDisplay::GraphicDisplay() :
	IOStream(writer),
	writer(this),
	draw(&modm::GraphicDisplay::setPixel),
	foregroundColor(glcd::Color::white()),
	backgroundColor(glcd::Color::black()),
	font(modm::accessor::asFlash(modm::font::FixedWidth5x8))
{
}

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::setColor(const glcd::Color& newColor)
{
//	if (newColor == glcd::Color::black()) {
//		draw = &modm::GraphicDisplay::clearPixel;
//	}
//	else {
//		draw = &modm::GraphicDisplay::setPixel;
//	}

	/* When using a multicolor display we don't need clearPixel(), or at least
	 * not the way it was implemented above. Maybe check if newColor equals
	 * backgroundColor.
	 * */
	draw = &modm::GraphicDisplay::setPixel;
	this->foregroundColor = newColor;
}

void
modm::GraphicDisplay::setBackgroundColor(const glcd::Color& newColor)
{
	this->backgroundColor = newColor;
}

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
{
	if (x1 == x2)
	{
		// x1|y1 must be the upper point
		if (y1 > y2) {
			modm::swap(y1, y2);
		}
		this->drawVerticalLine(glcd::Point(x1, y1), y2 - y1 + 1);
	}
	else if (y1 == y2)
	{
		// x1|y1 must be the left point
		if (x1 > x2) {
			modm::swap(x1, x2);
		}
		this->drawHorizontalLine(glcd::Point(x1, y1), x2 - x1 + 1);
	}
	else
	{
		// bresenham algorithm
		bool steep = abs(y2 - y1) > abs(x2 - x1);
		if (steep) {
			modm::swap(x1, y1);
			modm::swap(x2, y2);
		}
		if (x1 > x2) {
			modm::swap(x1, x2);
			modm::swap(y1, y2);
		}

		int16_t deltaX = x2 - x1;
		int16_t deltaY = abs(y2 - y1);
		int16_t error = deltaX / 2;

		int16_t yStep;
		int16_t y = y1;

		if (y1 < y2) {
			yStep = 1;
		}
		else {
			yStep = -1;
		}

		for (int_fast16_t x = x1; x <= x2; ++x)
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
modm::GraphicDisplay::drawHorizontalLine(glcd::Point start, uint16_t length)
{
	for (int_fast16_t i = start.getX(); i < static_cast<int16_t>(start.getX() + length); ++i) {
		(this->*draw)(i, start.getY());
	}
}

void
modm::GraphicDisplay::drawVerticalLine(glcd::Point start, uint16_t length)
{
	for (int_fast16_t i = start.getY(); i < static_cast<int16_t>(start.getY() + length); ++i) {
		(this->*draw)(start.getX(), i);
	}
}

void
modm::GraphicDisplay::drawRectangle(glcd::Point upperLeft,
		uint16_t width, uint16_t height)
{
	uint16_t x2 = upperLeft.getX() + width  - 1;
	uint16_t y2 = upperLeft.getY() + height - 1;

	this->drawHorizontalLine(upperLeft,  width);
	this->drawHorizontalLine(glcd::Point(upperLeft.getX(), y2), width);
	this->drawVerticalLine(upperLeft, height);
	this->drawVerticalLine(glcd::Point(x2, upperLeft.getY()), height);
}

void
modm::GraphicDisplay::drawRoundedRectangle(glcd::Point upperLeft,
		uint16_t width, uint16_t height, uint16_t radius)
{
	if (radius == 0) {
		this->drawRectangle(upperLeft, width, height);
	}

	const int16_t x = upperLeft.getX();
	const int16_t y = upperLeft.getY();

	int16_t x1 = 0;
	int16_t y1 = radius;
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

	this->drawHorizontalLine(glcd::Point(x + radius, y), width - (2 * radius));
	this->drawHorizontalLine(glcd::Point(x + radius, y + height), width - (2 * radius));
	this->drawVerticalLine(glcd::Point(x, y + radius), height - (2 * radius));
	this->drawVerticalLine(glcd::Point(x + width, y + radius), height - (2 * radius));
}

void
modm::GraphicDisplay::drawCircle(glcd::Point center, uint16_t radius)
{
	if (radius == 0) {
		return;
	}

	int16_t error = -radius;
	int16_t x = radius;
	int16_t y = 0;

	while (x > y)
	{
		this->drawCircle4(center, x, y);
		this->drawCircle4(center, y, x);

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
	this->drawCircle4(center, x, y);
}

void
modm::GraphicDisplay::drawCircle4(glcd::Point center, int16_t x, int16_t y)
{
	const int16_t cx = center.getX();
	const int16_t cy = center.getY();

	(this->*draw)(cx + x, cy + y);
	(this->*draw)(cx - x, cy - y);
	if (x != 0) {
		(this->*draw)(cx - x, cy + y);
	}
	if (y != 0) {
		(this->*draw)(cx + x, cy - y);
	}
}

void
modm::GraphicDisplay::drawEllipse(glcd::Point center, int16_t rx, int16_t ry)
{
	int32_t rx_2 = rx * rx;
	int32_t ry_2 = ry * ry;

	int16_t x = 0;
	int16_t y = ry;

	int32_t fx = 0;
	int32_t fy = rx_2 * 2 * ry;

	int32_t p = ry_2 - (rx_2 * ry) + (rx_2 + 2) / 4;

	drawCircle4(center, x, y);
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

		drawCircle4(center, x, y);
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

		drawCircle4(center, x, y);
	}
}

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::drawImage(glcd::Point upperLeft,
		modm::accessor::Flash<uint8_t> image)
{
	uint8_t width = image[0];
	uint8_t height = image[1];

	drawImageRaw(upperLeft, width, height,
			modm::accessor::Flash<uint8_t>(image.getPointer() + 2));
}

void
modm::GraphicDisplay::drawImageRaw(glcd::Point upperLeft,
		uint16_t width, uint16_t height,
		modm::accessor::Flash<uint8_t> data)
{
	uint16_t rows = (height + 7) / 8;
	for (uint16_t i = 0; i < width; i++)
	{
		for (uint16_t k = 0; k < rows; k++)
		{
			uint16_t byte = data[i + k * width];
			uint16_t rowHeight = height - k * 8;
			if (rowHeight > 8) {
				rowHeight = 8;
			}
			for (uint16_t j = 0; j < rowHeight; j++)
			{
				if (byte & 0x01) {
					this->setPixel(upperLeft.getX() + i, upperLeft.getY() + k * 8 + j);
				}
				else {
					this->clearPixel(upperLeft.getX() + i, upperLeft.getY() + k * 8 + j);
				}
				byte >>= 1;
			}
		}
	}
}
