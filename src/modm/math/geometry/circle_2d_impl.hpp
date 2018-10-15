/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009, Thorsten Lajewski
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_CIRCLE_2D_HPP
	#error	"Don't include this file directly, use 'circle_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
modm::Circle2D<T>::Circle2D() :
	center(), radius(0)
{
}

template <typename T>
modm::Circle2D<T>::Circle2D(const Vector<T, 2>& inCenter, T inRadius) :
	center(inCenter), radius(inRadius)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline const modm::Vector<T, 2>&
modm::Circle2D<T>::getCenter() const
{
	return this->center;
}

template <typename T>
inline void
modm::Circle2D<T>::setCenter(const Vector<T, 2>& point)
{
	this->center = point;
}

template <typename T>
inline T
modm::Circle2D<T>::getRadius() const
{
	return this->radius;
}

template <typename T>
inline void
modm::Circle2D<T>::setRadius(T newRadius)
{
	this->radius = newRadius;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Circle2D<T>::intersects(const Polygon2D<T>& polygon) const
{
	return polygon.intersects(*this);
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Circle2D<T>::getIntersections(const Circle2D& other,
		PointSet2D<T>& intersections) const
{
	Vector<T, 2> circleToCircle = other.center - this->center;
	WideType distanceSquared = circleToCircle.getLengthSquared();

	if (distanceSquared == 0 and (this->radius == other.radius))
	{
		// circles are equal and infinite intersections exist
		return true;
	}
	else if (distanceSquared > (this->radius + other.radius) * (this->radius + other.radius) or
			 distanceSquared < (this->radius - other.radius) * (this->radius - other.radius))
	{
		// no intersection, circles do not intersect
		return false;
	}
	else if (distanceSquared == (this->radius + other.radius) * (this->radius + other.radius))
	{
		// one solution, circles touch each other

		// calculate the intersection point
		float scale = static_cast<float>(this->radius) / static_cast<float>(this->radius + other.radius);

		intersections.append(this->center + scale * circleToCircle);
		return true;
	}

	T distance = circleToCircle.getLength();

	// 'point C' is the point where the line through the circle
	// intersection points crosses the line between the circle
	// centers.

	// Determine the distance from the center of this circle to point C
	float a = ((this->radius * this->radius) - (other.radius * other.radius) + distanceSquared) /
			(2.0f * distance);

	// Determine the coordinates of point C
	Vector<T, 2> c = this->center + circleToCircle * (a / distance);

	// Determine the distance from point 2 to either of the intersection points
	float h = std::sqrt((this->radius * this->radius) - (a * a));

	// Now determine the offsets of the intersection points from point 2
	Vector<T, 2> r = circleToCircle * (h / distance);	// TODO check this
	r = r.toOrthogonalVector();

	// Determine the absolute intersection points
	intersections.append(c + r);
	intersections.append(c - r);

	return true;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Circle2D<T>::getIntersections(const Line2D<T>& line,
		PointSet2D<T>& intersections) const
{
	return line.getIntersections(*this, intersections);
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Circle2D<T>::getIntersections(const LineSegment2D<T>& line,
		PointSet2D<T>& intersections) const
{
	return line.getIntersections(*this, intersections);
}
