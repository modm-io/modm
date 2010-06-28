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

#ifndef XPCC__LINE_2D_HPP
	#error	"Don't include this file directly, use 'line_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Line2D<T>::Line2D() :
	point(), directionVector()
{
}

template <typename T>
xpcc::Line2D<T>::Line2D(const Point2D<T>& point, const Vector2D<T>& vector) :
	point(point), directionVector(vector)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline void
xpcc::Line2D<T>::setPoint(const Point2D<T>& point)
{
	this->point = point;
}

template <typename T>
inline const xpcc::Point2D<T>&
xpcc::Line2D<T>::getPoint() const
{
	return this->point;
}

template <typename T>
inline void
xpcc::Line2D<T>::setDirectionVector(const Vector2D<T>& vector)
{
	this->directionVector = vector;
}

template <typename T>
inline const xpcc::Vector2D<T>&
xpcc::Line2D<T>::getDirectionVector() const
{
	return this->directionVector;
}
// ----------------------------------------------------------------------------
template <typename T>
T
xpcc::Line2D<T>::getDistanceTo(const Point2D<T>& point) const
{
	// vector from the base point of the line to the new point
	Vector2D<T> startToPoint(this->point, point);
	
	float c1 = Vector2D<T>::dotProduct(startToPoint, this->directionVector);
	float c2 = this->directionVector.getLengthSquared();
	
	float d = c1 / c2;
	
	// calculate the closest point
	Vector2D<T> closestPoint(this->directionVector);
	closestPoint.scale(d);
	closestPoint += this->point.toVector();
	
	// return the length of the vector from the closest point on the line
	// to the given point
	Vector2D<T> closestPointToPoint = point.toVector() - closestPoint;
	return closestPointToPoint.getLength();
}
