// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__CIRCLE_2D_HPP
	#error	"Don't include this file directly, use 'circle_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Circle2D<T>::Circle2D() :
	center(), radius(0)
{
}

template <typename T>
xpcc::Circle2D<T>::Circle2D(const Vector2D<T>& center, T radius) :
	center(center), radius(radius)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline const xpcc::Vector2D<T>&
xpcc::Circle2D<T>::getCenter() const
{
	return this->center;
}

template <typename T>
inline void
xpcc::Circle2D<T>::setCenter(const Vector2D<T>& point)
{
	this->center = point;
}

template <typename T>
inline T
xpcc::Circle2D<T>::getRadius() const
{
	return this->radius;
}

template <typename T>
inline void
xpcc::Circle2D<T>::setRadius(T radius)
{
	this->radius = radius;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Circle2D<T>::intersects(const Polygon2D<T>& polygon) const
{
	return polygon.intersects(*this);
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::Circle2D<T>::getIntersections(const Circle2D& other,
		PointSet2D<T>& intersections) const
{
	Vector2D<T> circleToCircle(this->center, other.center);
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
	Vector2D<T> c = this->center + circleToCircle * (a / distance);
	
	// Determine the distance from point 2 to either of the intersection points
	float h = std::sqrt((this->radius * this->radius) - (a * a));
	
	// Now determine the offsets of the intersection points from point 2
	Vector2D<T> r = circleToCircle.scale(h / distance);
	r = r.toOrthogonalVector();
	
	// Determine the absolute intersection points
	intersections.append(c + r);
	intersections.append(c - r);
	
	return true;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::Circle2D<T>::getIntersections(const Line2D<T>& line,
		PointSet2D<T>& intersections) const
{
	return line.getIntersections(*this, intersections);
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::Circle2D<T>::getIntersections(const LineSegment2D<T>& line,
		PointSet2D<T>& intersections) const
{
	return line.getIntersections(*this, intersections);
}
