/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2009, 2012, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_LINE_SEGMENT_2D_HPP
	#error	"Don't include this file directly, use 'line_segment_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::LineSegment2D<T>::LineSegment2D() :
	startPoint(), endPoint()
{
}

template<typename T>
modm::LineSegment2D<T>::LineSegment2D(const Vector<T, 2>& start, const Vector<T, 2>& end) :
	startPoint(start), endPoint(end)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline void
modm::LineSegment2D<T>::setStartPoint(const Vector<T, 2>& point)
{
	this->startPoint = point;
}

template <typename T>
inline const modm::Vector<T, 2>&
modm::LineSegment2D<T>::getStartPoint() const
{
	return this->startPoint;
}

template <typename T>
inline void
modm::LineSegment2D<T>::setEndPoint(const Vector<T, 2>& point)
{
	this->endPoint = point;
}

template <typename T>
inline const modm::Vector<T, 2>&
modm::LineSegment2D<T>::getEndPoint() const
{
	return this->endPoint;
}

template <typename T>
inline void
modm::LineSegment2D<T>::set(const Vector<T, 2>& start, const Vector<T, 2>& end)
{
	this->startPoint = start;
	this->endPoint = end;
}

// ----------------------------------------------------------------------------
template<typename T>
void
modm::LineSegment2D<T>::translate(const Vector<T, 2>& vector)
{
	this->startPoint.translate(vector);
	this->endPoint.translate(vector);
}

// ----------------------------------------------------------------------------
template<typename T>
T
modm::LineSegment2D<T>::getLength() const
{
	Vector<T, 2> directionVector = this->endPoint - this->startPoint;

	return directionVector.getLength();
}

// ----------------------------------------------------------------------------
template<typename T>
modm::Vector<T, 2>
modm::LineSegment2D<T>::getDirectionVector() const
{
	return endPoint - startPoint;
}

// ----------------------------------------------------------------------------
template<typename T>
const T
modm::LineSegment2D<T>::getDistanceTo(const Vector<T, 2>& point) const
{
	// vector from the base point of the line to the new point
	Vector<T, 2> startToPoint = point - this->startPoint;
	Vector<T, 2> directionVector = this->endPoint - this->startPoint;

	FloatType c1 = startToPoint.dot(directionVector);
	if (c1 <= 0)
	{
		// point is before the start point => calculate distance to start point
		return startToPoint.getLength();
	}

	FloatType c2 = directionVector.getLengthSquared();
	if (c2 <= c1)
	{
		// point is after the end point => calculate distance to end point
		Vector<T, 2> endToPoint = point - this->endPoint;
		return endToPoint.getLength();
	}

	FloatType d = c1 / c2;

	// calculate the closest point
	Vector<T, 2> closestPoint = this->startPoint + d * directionVector;

	// return the length of the vector from the closest point on the line
	// to the given point
	Vector<T, 2> closestPointToPoint = point - closestPoint;
	return closestPointToPoint.getLength();
}

// ----------------------------------------------------------------------------
template<typename T>
const modm::Vector<T, 2>
modm::LineSegment2D<T>::getClosestPointTo(const Vector<T, 2>& point) const
{
	// vector from the base point of the line to the new point
	Vector<T, 2> startToPoint = point - this->startPoint;
	Vector<T, 2> directionVector = this->endPoint - this->startPoint;

	FloatType c1 = startToPoint.dot(directionVector);
	if (c1 <= 0)
	{
		// point is before the start point
		return this->startPoint;
	}

	FloatType c2 = directionVector.getLengthSquared();
	if (c2 <= c1)
	{
		// point is after the end point
		return this->endPoint;
	}

	FloatType d = c1 / c2;

	// calculate the closest point
	return (this->startPoint + d * directionVector);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::LineSegment2D<T>::intersects(const LineSegment2D<T>& other) const
{
	return (((Vector<T, 2>::ccw(this->startPoint, this->endPoint, other.startPoint) *
			  Vector<T, 2>::ccw(this->startPoint, this->endPoint, other.endPoint)) <= 0) &&
			((Vector<T, 2>::ccw(other.startPoint, other.endPoint, this->startPoint) *
			  Vector<T, 2>::ccw(other.startPoint, other.endPoint, this->endPoint)) <= 0));
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::LineSegment2D<T>::intersects(const Polygon2D<T>& polygon) const
{
	return polygon.intersects(*this);
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::LineSegment2D<T>::getIntersections(const LineSegment2D& other,
		PointSet2D<T>& intersectionPoints) const
{
	modm::Vector<T, 2> ownDirectionVector = this->endPoint - this->startPoint;
	modm::Vector<T, 2> otherDirectionVector = other.endPoint - other.startPoint;
	modm::Vector<T, 2> connectionVector = this->startPoint - other.startPoint;

	WideType d = ownDirectionVector.cross(otherDirectionVector);
	if (d)
	{
		FloatType t2 = static_cast<FloatType>(ownDirectionVector.cross(connectionVector)) /
					   static_cast<FloatType>(d);

		if (0.f <= t2 and t2 <= 1.f)
		{
			FloatType t1 = static_cast<FloatType>(otherDirectionVector.cross(connectionVector)) /
					   static_cast<FloatType>(d);

			if (0.f <= t1 and t1 <= 1.f)
			{
				intersectionPoints.append(this->startPoint + ownDirectionVector * t1);
				return true;
			}
		}
	}
	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::LineSegment2D<T>::getIntersections(const Circle2D<T>& circle,
		PointSet2D<T>& intersectionPoints) const
{
	// Direction vector of line, from start to end
	modm::Vector<T, 2> directionVector = this->endPoint - this->startPoint;

	// vector from the center of the circle to line start
	modm::Vector<T, 2> circleToLine = this->startPoint - circle.center;

	WideType a = directionVector.dot(directionVector);
	WideType b = 2 * circleToLine.dot(directionVector);
	WideType c = circleToLine.dot(circleToLine) -
			static_cast<WideType>(circle.radius) * static_cast<WideType>(circle.radius);;

	WideType discriminant = (b * b - 4 * a * c);

	if (discriminant < 0)
	{
		// no intersections
		return false;
	}
	else
	{
		bool result = false;
		FloatType e = std::sqrt(discriminant);

		FloatType t1 = static_cast<FloatType>(-b - e) / static_cast<FloatType>(2 * a);
		if (0.f <= t1 and t1 <= 1.f) {
			intersectionPoints.append(this->startPoint + directionVector * t1);
			result = true;
		}

		if (discriminant == 0)
		{
			// the line is a tangent to the circle intersecting
			// it at only one point
			return result;
		}

		FloatType t2 = static_cast<FloatType>(-b + e) / static_cast<FloatType>(2 * a);
		if (0.f <= t2 and t2 <= 1.f) {
			intersectionPoints.append(this->startPoint + directionVector * t2);
			result = true;
		}
		return result;
	}
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::LineSegment2D<T>::getIntersections(const Polygon2D<T>& polygon,
		PointSet2D<T>& intersectionPoints) const
{
	// invoke intersection method of the polygon
	return polygon.getIntersections(*this, intersectionPoints);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::LineSegment2D<T>::operator == (const LineSegment2D &other) const
{
	return ((this->startPoint == other.startPoint) &&
			(this->endPoint == other.endPoint));
}

template<typename T>
bool
modm::LineSegment2D<T>::operator != (const LineSegment2D &other) const
{
	return ((this->startPoint != other.startPoint) ||
			(this->endPoint != other.endPoint));
}
