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

#include "point.hpp"
#include "rectangle.hpp"

#include <modm/math/geometry/circle_2d.hpp>
namespace modm::shape
{

class Rectangle;
class Circle
{
public:
	Point center;
	uint16_t radius;

	constexpr Circle() = default;

	/**
	 * @brief Construct a new Circle object
	 *
	 * @param center 	Center of the circle
	 * @param radius 	Radius of the circle
	 */
	constexpr Circle(Point center, uint16_t radius)
		: center(center), radius(radius)
	{}

	template <typename T>
	Circle(Circle2D<T> &circle2d) : center(circle2d.center), radius(circle2d.radius) {}
};
}  // namespace modm::shape