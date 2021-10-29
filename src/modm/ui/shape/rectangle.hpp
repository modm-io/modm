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

namespace modm::shape
{

class Rectangle
{
public:
	Point origin;
	Size size;

	constexpr Rectangle() = default;

	/**
	 * @brief Construct a new Rectangle object
	 *
	 * @param origin 	Top left corner of the Rectangle
	 * @param size 		Size x and y of the Rectangle
	 */
	constexpr Rectangle(Point origin, Size size) : origin(origin), size(size) {}

	Point
	getBottomRight() const {
		return origin + size;
	}

	Point
	getCenter() const {
		return origin + size / 2;
	}
};
}  // namespace modm::shape