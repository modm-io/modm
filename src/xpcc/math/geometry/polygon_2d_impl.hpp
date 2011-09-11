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

#ifndef XPCC__POLYGON_2D_HPP
	#error	"Don't include this file directly, use 'polygon_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Polygon2D<T>::Polygon2D(SizeType n) :
	PointSet2D<T>(n)
{
}

template <typename T>
xpcc::Polygon2D<T>::Polygon2D(const Polygon2D<T>& other) :
	PointSet2D<T>(other)
{
}

template <typename T>
xpcc::Polygon2D<T>&
xpcc::Polygon2D<T>::operator = (const Polygon2D<T>& other)
{
	this->points = other.points;
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Polygon2D<T>&
xpcc::Polygon2D<T>::operator << (const Vector<T, 2>& point)
{
	this->append(point);
	return *this;
}

// ----------------------------------------------------------------------------
template <typename T>
bool
xpcc::Polygon2D<T>::intersects(const Polygon2D& other) const
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
xpcc::Polygon2D<T>::intersects(const Circle2D<T>& circle) const
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
xpcc::Polygon2D<T>::intersects(const LineSegment2D<T>& segment) const
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
xpcc::Polygon2D<T>::getIntersections(const LineSegment2D<T>& segment, PointSet2D<T>& intersectionPoints) const
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
