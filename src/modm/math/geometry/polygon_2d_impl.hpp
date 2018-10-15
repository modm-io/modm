/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2015, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_POLYGON_2D_HPP
	#error	"Don't include this file directly, use 'polygon_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
modm::Polygon2D<T>::Polygon2D(SizeType n) :
	PointSet2D<T>(n)
{
}

template <typename T>
modm::Polygon2D<T>::Polygon2D(const Polygon2D<T>& other) :
	PointSet2D<T>(other)
{
}

template <typename T>
modm::Polygon2D<T>::Polygon2D(std::initializer_list<modm::Polygon2D<T>::PointType> init) :
	PointSet2D<T>(init)
{
}

template <typename T>
modm::Polygon2D<T>&
modm::Polygon2D<T>::operator = (const Polygon2D<T>& other)
{
	this->points = other.points;
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
modm::Polygon2D<T>&
modm::Polygon2D<T>::operator << (const modm::Polygon2D<T>::PointType& point)
{
	this->append(point);
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Polygon2D<T>::intersects(const Polygon2D& other) const
{
	SizeType n = this->points.getSize();
	SizeType m = other.points.getSize();

	for (SizeType i = 0; i < n; ++i)
	{
		for (SizeType k = 0; k < m; ++k)
		{
			LineSegment2D<T> lineSegmentOwn(this->points[i], this->points[(i + 1) % n]);
			LineSegment2D<T> lineSegmentOther(other.points[k], other.points[(k + 1) % m]);

			if (lineSegmentOwn.intersects(lineSegmentOther)) {
				return true;
			}
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Polygon2D<T>::intersects(const Circle2D<T>& circle) const
{
	SizeType n = this->points.getSize();
	for (SizeType i = 0; i < n; ++i)
	{
		LineSegment2D<T> segment(this->points[i], this->points[(i + 1) % n]);

		T distance = segment.getDistanceTo(circle.getCenter());
		if (distance <= circle.getRadius()) {
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Polygon2D<T>::intersects(const LineSegment2D<T>& segment) const
{
	SizeType n = this->points.getSize();
	for (SizeType i = 0; i < n; ++i)
	{
		LineSegment2D<T> ownSegment(this->points[i], this->points[(i + 1) % n]);

		if (segment.intersects(ownSegment)) {
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Polygon2D<T>::intersects(const Ray2D<T>& segment) const
{
	SizeType n = this->points.getSize();
	for (SizeType i = 0; i < n; ++i)
	{
		LineSegment2D<T> ownSegment(this->points[i], this->points[(i + 1) % n]);

		if (segment.intersects(ownSegment)) {
			return true;
		}
	}

	return false;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Polygon2D<T>::getIntersections(const LineSegment2D<T>& segment, PointSet2D<T>& intersectionPoints) const
{
	bool intersectionFound = false;

	SizeType n = this->points.getSize();
	for (SizeType i = 0; i < n; ++i)
	{
		LineSegment2D<T> ownSegment(this->points[i], this->points[(i + 1) % n]);

		if (segment.getIntersections(ownSegment, intersectionPoints)) {
			intersectionFound = true;
		}
	}

	return intersectionFound;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
modm::Polygon2D<T>::isInside(const modm::Polygon2D<T>::PointType& point)
{
	bool cw = true;
	bool ccw = true;

	SizeType n = this->points.getSize();
	for (SizeType i = 0; i < n; ++i) {
		int_fast8_t r = Vector<T, 2>::ccw(this->points[i], this->points[(i + 1) % n], point);

		switch (r) {
		case 0:
			// point is one of the lines on the edge of the polygon.
			return true;
			break;

		case 1:
			cw = false;
			break;

		case -1:
			ccw = false;
			break;
		}
	}

	return (cw || ccw);
}
