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

#ifndef XPCC__LINE_SEGMENT_2D_HPP
	#error	"Don't include this file directly, use 'line_segment_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::LineSegment2D<T>::LineSegment2D() :
	start(), end()
{
}

template<typename T>
xpcc::LineSegment2D<T>::LineSegment2D(const Point2D<T>& start, const Point2D<T>& end) :
	start(start), end(end)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline void
xpcc::LineSegment2D<T>::setStartPoint(const Point2D<T>& point)
{
	this->start = point;
}

template <typename T>
inline const xpcc::Point2D<T>&
xpcc::LineSegment2D<T>::getStartPoint() const
{
	return this->start;
}

template <typename T>
inline void
xpcc::LineSegment2D<T>::setEndPoint(const Point2D<T>& point)
{
	this->end = point;
}

template <typename T>
inline const xpcc::Point2D<T>&
xpcc::LineSegment2D<T>::getEndPoint() const
{
	return this->end;
}

template <typename T>
inline void
xpcc::LineSegment2D<T>::setPoints(const Point2D<T>& start, const Point2D<T>& end)
{
	this->start = start;
	this->end = end;
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::LineSegment2D<T>::getLength() const
{
	Vector2D<T> directionVector(this->start, this->end);
	
	return directionVector.getLength();
}

// ----------------------------------------------------------------------------
template<typename T>
const T
xpcc::LineSegment2D<T>::getDistanceTo(const Point2D<T>& point) const
{
	// vector from the base point of the line to the new point
	Vector2D<T> startToPoint(this->start, point);
	Vector2D<T> directionVector(this->start, this->end);
	
	FloatType c1 = Vector2D<T>::dotProduct(startToPoint, directionVector);
	if (c1 <= 0)
	{
		// point to before the start point => calculate distance to start point
		return startToPoint.getLength();
	}
	
	FloatType c2 = directionVector.getLengthSquared();
	if (c2 <= c1)
	{
		// point is after the end point => calculate distance to end point
		Vector2D<T> endToPoint(this->end, point);
		return endToPoint.getLength();
	}
	
	FloatType d = c1 / c2;
	
	// calculate the closest point
	Vector2D<T> closestPoint(directionVector);
	closestPoint.scale(d);
	closestPoint += this->start.toVector();
	
	// return the length of the vector from the closest point on the line
	// to the given point
	Vector2D<T> closestPointToPoint = point.toVector() - closestPoint;
	return closestPointToPoint.getLength();
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::LineSegment2D<T>::intersects(const LineSegment2D<T>& other)
{
	return (((Point2D<T>::ccw(this->start, this->end, other.start) *
			  Point2D<T>::ccw(this->start, this->end, other.end)) <= 0) &&
			((Point2D<T>::ccw(other.start, other.end, this->start) *
			  Point2D<T>::ccw(other.start, other.end, this->start)) <= 0));
}

// ----------------------------------------------------------------------------
template<typename U>
bool
xpcc::operator == (const LineSegment2D<U> &a, const LineSegment2D<U> &b)
{
	return ((a.start == b.start) && (a.end == b.end));
}

template<typename U>
bool
xpcc::operator != (const LineSegment2D<U> &a, const LineSegment2D<U> &b)
{
	return ((a.start != b.start) || (a.end != b.end));
}
