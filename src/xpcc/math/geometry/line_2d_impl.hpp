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
xpcc::Line2D<T>::Line2D(const Vector<T, 2>& point, const Vector<T, 2>& direction) :
	point(point), directionVector(direction)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline void
xpcc::Line2D<T>::setPoint(const Vector<T, 2>& point)
{
	this->point = point;
}

template <typename T>
inline const xpcc::Vector<T, 2>&
xpcc::Line2D<T>::getPoint() const
{
	return this->point;
}

template <typename T>
inline void
xpcc::Line2D<T>::setDirectionVector(const Vector<T, 2>& vector)
{
	this->directionVector = vector;
}

template <typename T>
inline const xpcc::Vector<T, 2>&
xpcc::Line2D<T>::getDirectionVector() const
{
	return this->directionVector;
}

template <typename T>
inline void
xpcc::Line2D<T>::set(const Vector<T, 2>& point, const Vector<T, 2>& direction)
{
	this->point = point;
	this->directionVector = direction;
}

// ----------------------------------------------------------------------------
template <typename T>
T
xpcc::Line2D<T>::getDistanceTo(const Vector<T, 2>& point) const
{
	// vector from the base point of the line to the new point
	Vector<T, 2> startToPoint = point - this->point;
	
	FloatType c1 = startToPoint.dot(this->directionVector);
	FloatType c2 = this->directionVector.getLengthSquared();
	
	FloatType d = c1 / c2;
	
	// calculate the closest point
	Vector<T, 2> closestPoint = this->point + d * this->directionVector;
	
	// return the length of the vector from the closest point on the line
	// to the given point
	return (point - closestPoint).getLength();
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::Line2D<T>::getIntersections(const Line2D& other,
		PointSet2D<T>& intersections) const
{
	xpcc::Vector<T, 2> connectionVector = this->point - other.point;
	
	WideType d = this->directionVector.cross(other.directionVector);
	if (d)
	{
		FloatType t1 = static_cast<FloatType>(other.directionVector.cross(connectionVector)) /
					   static_cast<FloatType>(d);
		
		intersections.append(this->point + this->directionVector * t1);
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::Line2D<T>::getIntersections(const Circle2D<T>& circle,
		PointSet2D<T>& intersections) const
{
	// vector from the center of the circle to line start
	xpcc::Vector<T, 2> circleToLine = this->point - circle.center;
	
	WideType a = 2 * this->directionVector.dot(this->directionVector);
	WideType b = 2 * circleToLine.dot(this->directionVector);
	WideType c = circleToLine.dot(circleToLine) - 
			static_cast<WideType>(circle.radius) * static_cast<WideType>(circle.radius);
	
	WideType discriminant = (b * b - 2 * a * c);
	
	if (discriminant < 0)
	{
		// no intersections
		return false;
	}
	else
	{
		FloatType e = std::sqrt(discriminant);
		
		FloatType t1 = static_cast<FloatType>(-b - e) / static_cast<FloatType>(a);
		intersections.append(this->point + this->directionVector * t1);
		
		if (discriminant == 0) {
			// the line is a tangent to the circle intersecting
			// it at only one point
			return true;
		}
		
		FloatType t2 = static_cast<FloatType>(-b + e) / static_cast<FloatType>(a);
		intersections.append(this->point + this->directionVector * t2);
		
		return true;
	}
}
