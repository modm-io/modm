// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Fabian Greif
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

#ifndef XPCC__RAY_2D_HPP
#define XPCC__RAY_2D_HPP

#include "geometric_traits.hpp"

#include "vector.hpp"
#include "point_set_2d.hpp"

namespace xpcc
{
	// forward declaration
	template <typename T>
	class Circle2D;
	
	template <typename T>
	class Polygon2D;
	
	template <typename T>
	class LineSegment2D;
	
	/**
	 * Ray.
	 * 
	 * A ray is composed of a starting point and a direction vector.
	 * 
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template <typename T = int16_t>
	class Ray2D
	{
	public:
		typedef typename GeometricTraits<T>::WideType WideType;
		typedef typename GeometricTraits<T>::FloatType FloatType;
		
	public:
		Ray2D();
		
		Ray2D(const Vector<T, 2>& start, const Vector<T, 2>& direction);
		
		/// Set the starting point of the ray
		inline void
		setStartPoint(const Vector<T, 2>& point);
		
		inline const Vector<T, 2>&
		getStartPoint() const;
		
		inline void
		setDirectionVector(const Vector<T, 2>& direction);
		
		inline const Vector<T, 2>&
		getDirectionVector() const;
		
		void
		translate(const Vector<T, 2>& vector);
		
		/**
		 * \brief	Length of the line segment
		 */
		T
		getLength() const;
		
		/// Shortest distance to a point
//		const T
//		getDistanceTo(const Vector<T, 2>& point) const;
//		
//		/// Calculate the point on the line segment closes to the given point
//		const Vector<T, 2>
//		getClosestPointTo(const Vector<T, 2>& point) const;
		
		/**
		 * \brief	Check if two line segments intersect
		 * 
		 * Uses Ray2D::ccw() to check if any intersection exists.
		 */
		bool
		intersects(const LineSegment2D<T>& line) const;
//		
//		/// Check if a intersection exists
//		bool
//		intersects(const Polygon2D<T>& polygon) const;
//		
//		/**
//		 * \brief	Calculate the intersection point
//		 */
//		bool
//		getIntersections(const LineSegment2D& other,
//				PointSet2D<T>& intersectionPoints) const;
//		
//		/**
//		 * \brief	Calculate the intersection point(s)
//		 * 
//		 * \see		http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
//		 */
//		bool
//		getIntersections(const Circle2D<T>& circle,
//				PointSet2D<T>& intersectionPoints) const;
//		
//		bool
//		getIntersections(const Polygon2D<T>& polygon,
//				PointSet2D<T>& intersectionPoints) const;
		
		bool
		ccw(const xpcc::Vector<T, 2>& point);
		
		bool
		operator == (const Ray2D &other) const;
		
		bool
		operator != (const Ray2D &other) const;
		
	protected:
		xpcc::Vector<T, 2> basePoint;
		xpcc::Vector<T, 2> direction;
	};
}

#include "circle_2d.hpp"
#include "polygon_2d.hpp"
#include "line_segment_2d.hpp"

#include "ray_2d_impl.hpp"

#endif // XPCC__RAY_2D_HPP
