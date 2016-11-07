/*
 * Copyright (c) 2010-2011, 2013, Fabian Greif
 * Copyright (c) 2012-2013, Niklas Hauser
 * Copyright (c) 2013, Thorsten Lajewski
 * Copyright (c) 2013, None
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
xpcc::GraphicDisplay::fillRectangle(glcd::Point upperLeft,
		uint16_t width, uint16_t height)
{
	for (uint16_t i = upperLeft.getX();
			(i < upperLeft.getX() + width) && (i < getWidth());
			++i)
	{
		for (uint16_t k = upperLeft.getY();
				(k < upperLeft.getY() + height) && (k < getHeight());
				++k)
		{
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
