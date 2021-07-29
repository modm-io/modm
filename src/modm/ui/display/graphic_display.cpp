/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012-2014, 2017, Niklas Hauser
 * Copyright (c) 2014, Daniel Krebs
 * Copyright (c) 2016, Antal Szabó
 * Copyright (c) 2017, Christopher Durand
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "graphic_display.hpp"

#include <cstdlib>
#include <modm/math/utils/bit_operation.hpp>

#include "font/fixed_width_5x8.hpp"

using namespace modm::display;

// ----------------------------------------------------------------------------
modm::GraphicDisplay::GraphicDisplay()
	: IOStream(writer), writer(this), font(modm::accessor::asFlash(modm::font::FixedWidth5x8))
{}

#include <modm/debug/logger.hpp>

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::drawLine(Point start, Point end)
{
	if (start.x == end.x)
	{
		// start.x|start.y must be the upper point
		if (start.y > end.y) { modm::swap(start.y, end.y); }
		this->drawVerticalLine(start, end.y - start.y + 1);
	} else if (start.y == end.y)
	{
		// start.x|start.y must be the left point
		if (start.x > end.x) { modm::swap(start.x, end.x); }
		this->drawHorizontalLine(start, end.x - start.x + 1);
	} else
	{
		// bresenham algorithm
		bool steep = abs(end.y - start.y) > abs(end.x - start.x);
		if (steep)
		{
			modm::swap(start.x, start.y);
			modm::swap(end.x, end.y);
		}

		if (start.x > end.x)
		{
			modm::swap(start.x, end.x);
			modm::swap(start.y, end.y);
		}

		int16_t deltaX = end.x - start.x;
		int16_t deltaY = abs(end.y - start.y);
		int16_t error = deltaX / 2;

		int16_t yStep;
		int16_t y = start.y;

		if (start.y < end.y)
			yStep = 1;
		else
			yStep = -1;

		for (int_fast16_t x = start.x; x <= end.x; ++x)
		{
			if (steep)
				this->setPixel({y, x});
			else
				this->setPixel({x, y});

			error = error - deltaY;

			if (error < 0)
			{
				y += yStep;
				error += deltaX;
			}
		}
	}
}

void
modm::GraphicDisplay::drawHorizontalLine(Point start, uint16_t length)
{
	uint16_t x_end = start.x + length;
	while(start.x < x_end)
		this->setPixel(start);
}

void
modm::GraphicDisplay::drawVerticalLine(Point start, uint16_t length)
{
	uint16_t y_end = start.y + length;
	while(start.y < y_end)
		this->setPixel(start);
}

void
modm::GraphicDisplay::drawRectangle(Point origin, uint16_t width, uint16_t height)
{
	uint16_t end_x = origin.x + width - 1;
	uint16_t end_y = origin.y + height - 1;

	this->drawHorizontalLine(origin, width);
	this->drawHorizontalLine({origin.x, end_y}, width);
	this->drawVerticalLine(origin, height);
	this->drawVerticalLine({end_x, origin.y}, height);
}

void
modm::GraphicDisplay::fillRectangle(Point start, uint16_t width, uint16_t height)
{
	for (uint16_t i = start.x; (i < start.x + width) && (i < getWidth()); ++i)
		for (uint16_t k = start.y; (k < start.y + height) && (k < getHeight()); ++k)
			this->setPixel({i, k});
}

void
modm::GraphicDisplay::drawRoundedRectangle(Point start, uint16_t width, uint16_t height,
										   uint16_t radius)
{
	if (radius == 0) { this->drawRectangle(start, width, height); }

	const int16_t x = start.x;
	const int16_t y = start.y;

	int16_t x1 = 0;
	int16_t y1 = radius;
	int16_t f = 3 - 2 * radius;

	while (x1 <= y1)
	{
		this->setPixel({x + radius - start.x, y + radius - start.y});
		this->setPixel({x + radius - start.x, y + height - radius + start.y});
		this->setPixel({x + radius - start.y, y + radius - start.x});
		this->setPixel({x + radius - start.y, y + height - radius + start.x});

		this->setPixel({x + width - radius + start.x, y + radius - start.y});
		this->setPixel({x + width - radius + start.x, y + height - radius + start.y});
		this->setPixel({x + width - radius + start.y, y + radius - start.x});
		this->setPixel({x + width - radius + start.y, y + height - radius + start.x});

		if (f < 0)
		{
			f += (4 * start.x + 6);
		} else
		{
			f += (4 * (start.x - start.y) + 10);
			start.y--;
		}
		start.x++;
	}

	this->drawHorizontalLine(Point(x + radius, y), width - (2 * radius));
	this->drawHorizontalLine(Point(x + radius, y + height), width - (2 * radius));
	this->drawVerticalLine(Point(x, y + radius), height - (2 * radius));
	this->drawVerticalLine(Point(x + width, y + radius), height - (2 * radius));
}

void
modm::GraphicDisplay::drawCircle(Point center, uint16_t radius)
{
	if (radius == 0) return;

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
modm::GraphicDisplay::fillCircle(Point center, uint16_t radius)
{
	if (radius < 2) return;

	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;

	this->drawVerticalLine(Point(center.x, center.y - radius), 2 * radius);

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

		this->drawVerticalLine(center + Point(x, -y), 2 * y);
		this->drawVerticalLine(center + Point(y, -x), 2 * x);
		this->drawVerticalLine(center + Point(-x, -y), 2 * y);
		this->drawVerticalLine(center + Point(-y, -x), 2 * x);
	}
}

void
modm::GraphicDisplay::drawEllipse(Point center, int16_t rx, int16_t ry)
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

		if (p < 0)
		{
			p += (fx + ry_2);
		} else
		{
			y--;
			fy -= rx_2 * 2;
			p += (fx + ry_2 - fy);
		}

		drawCircle4(center, x, y);
	}

	p = ((ry_2 * (4 * x * x + 4 * x + 1) / 2) + 2 * (rx_2 * (y - 1) * (y - 1)) - 2 * (rx_2 * ry_2) +
		 1) /
		2;

	while (y > 0)
	{
		y--;
		fy -= rx_2 * 2;

		if (p >= 0)
		{
			p += (rx_2 - fy);
		} else
		{
			x++;
			fx += ry_2 * 2;
			p += (fx + rx_2 - fy);
		}

		drawCircle4(center, x, y);
	}
}

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::drawImage(Point start, modm::accessor::Flash<uint8_t> image)
{
	uint8_t width = image[0];
	uint8_t height = image[1];

	drawImageRaw(start, width, height, modm::accessor::Flash<uint8_t>(image.getPointer() + 2));
}

void
modm::GraphicDisplay::drawImageRaw(Point start,
	uint16_t width, uint16_t height, modm::accessor::Flash<uint8_t> data)
{
	uint16_t rows = (height + 7) / 8;
	for (uint16_t i = 0; i < width; i++)
	{
		for (uint16_t k = 0; k < rows; k++)
		{
			uint16_t byte = data[i + k * width];
			uint16_t rowHeight = height - k * 8;
			if (rowHeight > 8) { rowHeight = 8; }
			for (uint16_t j = 0; j < rowHeight; j++)
			{
				if (byte & 0x01)
					this->setPixel({start.x + i, start.y + k * 8 + j});

				byte >>= 1;
			}
		}
	}
}

void
modm::GraphicDisplay::drawCircle4(Point center, int16_t x, int16_t y)
{
	this->setPixel(center + Point(x, y));
	this->setPixel(center - Point(x, y));
	if (x != 0) this->setPixel(center + Point(-x, y));
	if (y != 0) this->setPixel(center + Point(x, -y));
}