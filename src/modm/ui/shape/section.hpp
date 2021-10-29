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

#include "circle.hpp"
#include "point.hpp"
#include "rectangle.hpp"

namespace modm::shape
{

// forward declarations for convertion constructors
class Rectangle;
class Circle;

class Section
{
public:
	Point topLeft, bottomRight;

	constexpr Section() = default;

	constexpr Section(Point topLeft, Point bottomRight) : topLeft(topLeft), bottomRight(bottomRight)
	{}

	// Conversion constructors
	constexpr Section(Rectangle rectangle)
		: topLeft(rectangle.origin), bottomRight(rectangle.origin + rectangle.size)
	{}

/* 	constexpr Section(Circle circle)
	{
		Point half_size(circle.radius, circle.radius);
		topLeft = circle.center - half_size;
		bottomRight = circle.center + half_size;
	} */

	constexpr Point
	getTopRight() const {
		return {bottomRight.x, topLeft.y};
	}

	constexpr Point
	getBottomLeft() const {
		return {topLeft.x, bottomRight.y};
	}

	constexpr uint16_t
	getWidth() const
	{ return bottomRight.x - topLeft.x; }

	constexpr uint16_t
	getHeight() const
	{ return bottomRight.y - topLeft.y; }
};
}  // namespace modm::shape