/*
 * Copyright (c) 2021, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "buffer.hpp"

using namespace modm::shape;
using namespace modm::color;

namespace modm::graphic {

template<ColorPlanar C, Size R>
void
BufferMemory<C, R>::drawBlind(const Point& point)
{
	buffer[point.y][point.x] = this->color;
}

template<ColorPlanar C, Size R>
void
// TODO make const HLine ... but benchmark with const and
BufferMemory<C, R>::drawBlind(const HLine& hline)
{
	std::fill(&buffer[hline.start.y][hline.start.x], &buffer[hline.start.y][hline.end_x], this->color);
}

template<ColorPlanar C, Size R>
void
BufferMemory<C, R>::drawBlind(const VLine& vline)
{
	shape::Point scanner = vline.start;
	while (scanner.y < vline.end_y)
	{
		drawBlind(scanner);
		scanner.y++;
	}
}

template<ColorPlanar C, Size R>
void
BufferMemory<C, R>::drawBlind(const Section& section)
{
	Point scanner = section.topLeft;
	while (scanner.y < section.bottomRight.y)
	{
		this->drawBlind(HLine(scanner, section.bottomRight.x));
		scanner.y++;
	}
}

}