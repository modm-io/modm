/*
 * Copyright (c) 2013, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_RAY_2D_HPP
	#error	"Don't include this file directly, use 'ray_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
modm::Ray2D<T>::Ray2D() :
	basePoint(), direction()
{
}

template<typename T>
modm::Ray2D<T>::Ray2D(const Vector<T, 2>& start, const Vector<T, 2>& direction) :
	basePoint(start), direction(direction)
{
}

// ----------------------------------------------------------------------------
template <typename T>
inline void
modm::Ray2D<T>::setStartPoint(const Vector<T, 2>& point)
{
	this->basePoint = point;
}

template <typename T>
inline const modm::Vector<T, 2>&
modm::Ray2D<T>::getStartPoint() const
{
	return this->basePoint;
}

template <typename T>
inline void
modm::Ray2D<T>::setDirectionVector(const Vector<T, 2>& direction)
{
	this->direction = direction;
}

// ----------------------------------------------------------------------------
template<typename T>
const modm::Vector<T, 2>&
modm::Ray2D<T>::getDirectionVector() const
{
	return direction;
}
//
//// ----------------------------------------------------------------------------
//template<typename T>
//void
//modm::Ray2D<T>::translate(const Vector<T, 2>& vector)
//{
//	this->startPoint.translate(vector);
//	this->endPoint.translate(vector);
//}
//
//// ----------------------------------------------------------------------------
//template<typename T>
//T
//modm::Ray2D<T>::getLength() const
//{
//	Vector<T, 2> directionVector = this->endPoint - this->startPoint;
//
//	return directionVector.getLength();
//}
//
//// ----------------------------------------------------------------------------
//template<typename T>
//const T
//modm::Ray2D<T>::getDistanceTo(const Vector<T, 2>& point) const
//{
//	// vector from the base point of the line to the new point
//	Vector<T, 2> startToPoint = point - this->startPoint;
//	Vector<T, 2> directionVector = this->endPoint - this->startPoint;
//
//	FloatType c1 = startToPoint.dot(directionVector);
//	if (c1 <= 0)
//	{
//		// point is before the start point => calculate distance to start point
//		return startToPoint.getLength();
//	}
//
//	FloatType c2 = directionVector.getLengthSquared();
//	if (c2 <= c1)
//	{
//		// point is after the end point => calculate distance to end point
//		Vector<T, 2> endToPoint = point - this->endPoint;
//		return endToPoint.getLength();
//	}
//
//	FloatType d = c1 / c2;
//
//	// calculate the closest point
//	Vector<T, 2> closestPoint = this->startPoint + d * directionVector;
//
//	// return the length of the vector from the closest point on the line
//	// to the given point
//	Vector<T, 2> closestPointToPoint = point - closestPoint;
//	return closestPointToPoint.getLength();
//}
//
//// ----------------------------------------------------------------------------
//template<typename T>
//const modm::Vector<T, 2>
//modm::Ray2D<T>::getClosestPointTo(const Vector<T, 2>& point) const
//{
//	// vector from the base point of the line to the new point
//	Vector<T, 2> startToPoint = point - this->startPoint;
//	Vector<T, 2> directionVector = this->endPoint - this->startPoint;
//
//	FloatType c1 = startToPoint.dot(directionVector);
//	if (c1 <= 0)
//	{
//		// point is before the start point
//		return this->startPoint;
//	}
//
//	FloatType c2 = directionVector.getLengthSquared();
//	if (c2 <= c1)
//	{
//		// point is after the end point
//		return this->endPoint;
//	}
//
//	FloatType d = c1 / c2;
//
//	// calculate the closest point
//	return (this->startPoint + d * directionVector);
//}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Ray2D<T>::intersects(const LineSegment2D<T>& line) const
{
	// vector from the base point of the line to the new point
	Vector<T, 2> startToPoint = line.getStartPoint() - this->basePoint;
	Vector<T, 2> endToPoint = line.getEndPoint() - this->basePoint;
	Vector<T, 2> dt = this->direction.toOrthogonalVector();

	if ((startToPoint.dot(dt) * endToPoint.dot(dt)) < 0) {
		// Points are on different sides of the ray (interpreted as
		// continuous line)

		Vector<T, 2> pointToStart = this->basePoint - line.getStartPoint();
		Vector<T, 2> lt = line.getDirectionVector().toOrthogonalVector();
		if ((pointToStart.dot(lt) * this->direction.dot(lt)) < 0) {
			// Point and
			return true;
		}
	}

	return false;
}

//// ----------------------------------------------------------------------------
//template<typename T>
//bool
//modm::Ray2D<T>::intersects(const Polygon2D<T>& polygon) const
//{
//	return polygon.intersects(*this);
//	''
//}
//
//// ----------------------------------------------------------------------------
//template <typename T>
//bool
//modm::Ray2D<T>::getIntersections(const Ray2D& other,
//		PointSet2D<T>& intersectionPoints) const
//{
//	modm::Vector<T, 2> ownDirectionVector = this->endPoint - this->startPoint;
//	modm::Vector<T, 2> otherDirectionVector = other.endPoint - other.startPoint;
//	modm::Vector<T, 2> connectionVector = this->startPoint - other.startPoint;
//
//	WideType d = ownDirectionVector.cross(otherDirectionVector);
//	if (d)
//	{
//		FloatType t2 = static_cast<FloatType>(ownDirectionVector.cross(connectionVector)) /
//					   static_cast<FloatType>(d);
//
//		if (0.f <= t2 and t2 <= 1.f)
//		{
//			FloatType t1 = static_cast<FloatType>(otherDirectionVector.cross(connectionVector)) /
//					   static_cast<FloatType>(d);
//
//			if (0.f <= t1 and t1 <= 1.f)
//			{
//				intersectionPoints.append(this->startPoint + ownDirectionVector * t1);
//				return true;
//			}
//		}
//	}
//	return false;
//}
//
//// ----------------------------------------------------------------------------
//template <typename T>
//bool
//modm::Ray2D<T>::getIntersections(const Circle2D<T>& circle,
//		PointSet2D<T>& intersectionPoints) const
//{
//	// Direction vector of line, from start to end
//	modm::Vector<T, 2> directionVector = this->endPoint - this->startPoint;
//
//	// vector from the center of the circle to line start
//	modm::Vector<T, 2> circleToLine = this->startPoint - circle.center;
//
//	WideType a = directionVector.dot(directionVector);
//	WideType b = 2 * circleToLine.dot(directionVector);
//	WideType c = circleToLine.dot(circleToLine) -
//			static_cast<WideType>(circle.radius) * static_cast<WideType>(circle.radius);;
//
//	WideType discriminant = (b * b - 4 * a * c);
//
//	if (discriminant < 0)
//	{
//		// no intersections
//		return false;
//	}
//	else
//	{
//		bool result = false;
//		FloatType e = std::sqrt(discriminant);
//
//		FloatType t1 = static_cast<FloatType>(-b - e) / static_cast<FloatType>(2 * a);
//		if (0.f <= t1 and t1 <= 1.f) {
//			intersectionPoints.append(this->startPoint + directionVector * t1);
//			result = true;
//		}
//
//		if (discriminant == 0)
//		{
//			// the line is a tangent to the circle intersecting
//			// it at only one point
//			return result;
//		}
//
//		FloatType t2 = static_cast<FloatType>(-b + e) / static_cast<FloatType>(2 * a);
//		if (0.f <= t2 and t2 <= 1.f) {
//			intersectionPoints.append(this->startPoint + directionVector * t2);
//			result = true;
//		}
//		return result;
//	}
//}
//
//// ----------------------------------------------------------------------------
//template <typename T>
//bool
//modm::Ray2D<T>::getIntersections(const Polygon2D<T>& polygon,
//		PointSet2D<T>& intersectionPoints) const
//{
//	// invoke intersection method of the polygon
//	return polygon.getIntersections(*this, intersectionPoints);
//}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Ray2D<T>::ccw(const modm::Vector<T, 2>& point)
{
	// vector from the base point of the line to the new point
	Vector<T, 2> startToPoint = point - this->basePoint;
	Vector<T, 2> t = direction.toOrthogonalVector();

	FloatType c1 = startToPoint.dot(t);

	return (c1 <= 0);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
modm::Ray2D<T>::operator == (const Ray2D &other) const
{
	return ((this->basePoint == other.basePoint) &&
			(this->direction == other.direction));
}

template<typename T>
bool
modm::Ray2D<T>::operator != (const Ray2D &other) const
{
	return ((this->basePoint != other.basePoint) ||
			(this->direction != other.direction));
}
