/*
 * Copyright (c) 2009, Martin Rosekeit
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

#ifndef MODM_LINE_2D_HPP
	#error	"Don't include this file directly, use 'line_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
modm::Line2D<T>::Line2D() :
	point(), directionVector()
{
}

template <typename T>
modm::Line2D<T>::Line2D(const Vector<T, 2>& point, const Vector<T, 2>& direction) :
	point(point), directionVector(direction)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline void
modm::Line2D<T>::setPoint(const Vector<T, 2>& point)
{
	this->point = point;
}

template <typename T>
inline const modm::Vector<T, 2>&
modm::Line2D<T>::getPoint() const
{
	return this->point;
}

template <typename T>
inline void
modm::Line2D<T>::setDirectionVector(const Vector<T, 2>& vector)
{
	this->directionVector = vector;
}

template <typename T>
inline const modm::Vector<T, 2>&
modm::Line2D<T>::getDirectionVector() const
{
	return this->directionVector;
}

template <typename T>
inline void
modm::Line2D<T>::set(const Vector<T, 2>& point, const Vector<T, 2>& direction)
{
	this->point = point;
	this->directionVector = direction;
}

// ----------------------------------------------------------------------------
template <typename T>
T
modm::Line2D<T>::getDistanceTo(const Vector<T, 2>& point) const
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
modm::Line2D<T>::getIntersections(const Line2D& other,
		PointSet2D<T>& intersections) const
{
	modm::Vector<T, 2> connectionVector = this->point - other.point;

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
modm::Line2D<T>::getIntersections(const Circle2D<T>& circle,
		PointSet2D<T>& intersections) const
{
	// vector from the center of the circle to line start
	modm::Vector<T, 2> circleToLine = this->point - circle.center;

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
