/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Hans Schily
 * Copyright (c) 2013, Thorsten Lajewski
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "graphic_display.hpp"

// ----------------------------------------------------------------------------
void
modm::GraphicDisplay::fillRectangle(glcd::Point start,
		uint16_t width, uint16_t height)
{
	for (uint16_t i = start.x; (i < start.x + width) && (i < getWidth()); ++i)
		for (uint16_t k = start.y; (k < start.y + height) && (k < getHeight()); ++k)
			this->setPixel(i, k);
}

void
modm::GraphicDisplay::fillCircle(glcd::Point center, uint16_t radius)
{
	int16_t f = 1 - radius;
	int16_t ddF_x = 0;
	int16_t ddF_y = -2 * radius;
	uint16_t x = 0;
	uint16_t y = radius;

	this->drawVerticalLine(glcd::Point(center.x, center.y - radius), 2 * radius);

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

		this->drawVerticalLine(glcd::Point(center.x + x, center.y - y), 2 * y);
		this->drawVerticalLine(glcd::Point(center.x + y, center.y - x), 2 * x);
		this->drawVerticalLine(glcd::Point(center.x - x, center.y - y), 2 * y);
		this->drawVerticalLine(glcd::Point(center.x - y, center.y - x), 2 * x);
	}
}
