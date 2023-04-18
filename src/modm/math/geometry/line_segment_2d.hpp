/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2022, Thomas Sommer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------
#pragma once

#include "geometric_traits.hpp"
#include "point_set_2d.hpp"
#include "vector.hpp"

namespace modm
{
// forward declaration
template<typename T>
class Circle2D;

template<typename T>
class Polygon2D;

/**
 * \brief	Line segment
 *
 * \author	Fabian Greif
 * \ingroup	modm_math_geometry
 */
template<typename T = int16_t>
class LineSegment2D
{
public:
	using WideType = GeometricTraits<T>::WideType;
	using FloatType = GeometricTraits<T>::FloatType;

public:
	modm::Vector<T, 2> startPoint, endPoint;

	constexpr LineSegment2D() = default;

	constexpr LineSegment2D(const Vector<T, 2>& start, const Vector<T, 2>& end)
		: startPoint(start), endPoint(end)
	{}

	constexpr bool operator==(const LineSegment2D& other) const = default;

	constexpr LineSegment2D& operator+=(const Vector<T, 2>& vector)
	{
		startPoint += vector;
		endPoint += vector;
		return *this;
	}

	constexpr LineSegment2D& operator-=(const Vector<T, 2>& vector)
	{
		startPoint -= vector;
		endPoint -= vector;
		return *this;
	}

	constexpr Vector<T, 2>
	getDirectionVector() const
	{ return endPoint - startPoint; }

	constexpr T
	getLength() const
	{
		Vector<T, 2> directionVector = this->endPoint - this->startPoint;
		return directionVector.getLength();
	}

	/// Shortest distance to a point
	constexpr T
	getDistanceTo(const Vector<T, 2>& point) const;

	/// Calculate the point on the line segment closes to the given point
	constexpr Vector<T, 2>
	getClosestPointTo(const Vector<T, 2>& point) const;

	/**
	 * \brief	Check if two line segments intersect
	 *
	 * Uses Vector2D::ccw() to check if any intersection exists.
	 */
	constexpr bool
	intersects(const LineSegment2D& other) const;

	constexpr bool
	intersects(const Polygon2D<T>& polygon) const
	{ return polygon.intersects(*this); }

	/// Calculate the intersection point
	constexpr bool
	getIntersections(const LineSegment2D& other, PointSet2D<T>& intersectionPoints) const;

	/**
	 * \brief	Calculate the intersection point(s)
	 * \see		http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
	 */
	constexpr bool
	getIntersections(const Circle2D<T>& circle, PointSet2D<T>& intersectionPoints) const;

	constexpr bool
	getIntersections(const Polygon2D<T>& polygon, PointSet2D<T>& intersectionPoints) const
	{ return polygon.getIntersections(*this, intersectionPoints); }

	// deprecated setters and getters
	[[deprecated("Assign public member directly!")]]
	void setStartPoint(const Vector<T, 2>& point)
	{ this->startPoint = point; }

	[[deprecated("Assign public member directly!")]]
	const Vector<T, 2>& getStartPoint() const
	{ return this->startPoint; }

	[[deprecated("Assign public member directly!")]]
	void setEndPoint(const Vector<T, 2>& point)
	{ this->endPoint = point; }

	[[deprecated("Assign public member directly!")]]
	const Vector<T, 2>& getEndPoint() const
	{ return this->endPoint; }

	[[deprecated("Assign public member directly!")]]
	void set(const Vector<T, 2>& start, const Vector<T, 2>& end)
	{
		this->startPoint = start;
		this->endPoint = end;
	}

	// deprecated translate
	[[deprecated("Use LineSegment2D<T>::operator+= instead!")]]
	void translate(const Vector<T, 2>& vector)
	{ operator+=(vector); }

};
}  // namespace modm

#include "circle_2d.hpp"
#include "line_segment_2d_impl.hpp"
#include "polygon_2d.hpp"