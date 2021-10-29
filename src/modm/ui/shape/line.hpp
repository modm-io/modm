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

#include "section.hpp"

#include <modm/math/geometry/line_2d.hpp>
namespace modm::shape
{

class Section;

class Line
{
public:
	Point start, end;

	constexpr Line() = default;

	/**
	 * @brief Construct a new Line object
	 *
	 * @param start		Start point of the line
	 * @param end 		End point of the line
	 */
	constexpr Line(Point start, Point end) : start(start), end(end) {}

	// Convertion
	// template<typename T>
	// Line(Line2D<T> &line2d)

	void
	clip(Section section) {
		// FIXME Not working when line doesn't intersect with section at all

		const Point axis = end - start;
		start = clipPoint(start, section, axis);
		end = clipPoint(end, section, axis);
	}

	constexpr bool
	isHorizontal() const
	{ return start.y == end.y; }

	constexpr bool
	isVertical() const
	{ return start.x == end.x; }

private:
	// Moves point along axis to make it intersect with section
	Point
	clipPoint(Point point, const Section section, const Point axis)
	{
		if (point.x < section.topLeft.x)
		{
			point.y += (section.topLeft.x - point.x) * axis.y / axis.x;
			point.x = section.topLeft.x;
		} else if (point.x >= section.bottomRight.x)
		{
			point.y += (section.bottomRight.x - point.x) * axis.y / axis.x;
			point.x = section.bottomRight.x - 1;
		}

		if (point.y < section.topLeft.y)
		{
			point.x += (section.topLeft.y - point.y) * axis.x / axis.y;
			point.y = section.topLeft.y;
		} else if (point.y >= section.bottomRight.y)
		{
			point.x += (section.bottomRight.y - point.y) * axis.x / axis.y;
			point.y = section.bottomRight.y - 1;
		}

		return point;
	}
};

class HLine
{
public:
	Point start;
	uint16_t end_x;

	constexpr HLine() = default;
	constexpr HLine(Point start, uint16_t end_x) : start(start), end_x(end_x) {}

	// Conversion constructor
	constexpr HLine(Line line) : start(line.start), end_x(line.end.x) {}
};

class VLine
{
public:
	Point start;
	uint16_t end_y;

	constexpr VLine() = default;
	constexpr VLine(Point start, uint16_t end_y) : start(start), end_y(end_y) {}

	// Conversion constructor
	constexpr VLine(Line line) : start(line.start), end_y(line.end.y) {}
};
}  // namespace modm