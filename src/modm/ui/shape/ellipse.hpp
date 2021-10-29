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

#include "rectangle.hpp"
#include "section.hpp"

namespace modm::shape
{

class Rectangle;
class Section;

class Ellipse
{
public:
	Point center;
	uint16_t radius_x, radius_y;

	constexpr Ellipse() = default;

	constexpr Ellipse(Point center, uint16_t radius_x, uint16_t radius_y)
		: center(center), radius_x(radius_x), radius_y(radius_y)
	{}
};
}  // namespace modm::shape