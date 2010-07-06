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
#define XPCC__CIRCLE_2D_HPP

#include <cmath>
#include "geometric_traits.hpp"

#include "vector_2d.hpp"
#include "point_set_2d.hpp"

namespace xpcc
{
	// forward declaration
	template <typename T>
	class Line2D;
	
	template <typename T>
	class LineSegment2D;
	
	template <typename T>
	class Polygon2D;
	
	/**
	 * \brief	Circle
	 * 
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template <typename T>
	class Circle2D
	{
		typedef typename GeometricTraits<T>::WideType WideType;
		typedef typename GeometricTraits<T>::FloatType FloatType;
		
	public:
		Circle2D();
		
		Circle2D(const Vector2D<T>& center, T radius);
		
		inline const Vector2D<T>&
		getCenter() const;
		
		inline void
		setCenter(const Vector2D<T>& point);
		
		inline T
		getRadius() const;
		
		inline void
		setRadius(T radius);
		
		/// Check if a intersection exists
		bool
		intersects(const Polygon2D<T>& polygon) const;
		
		/**
		 * \brief	Calculate intersection point(s)
		 * 
		 * \param[in]	other	Other Circle
		 * \param[out]	intersectionPoint	Intersection points
		 * 
		 * \see		http://local.wasp.uwa.edu.au/~pbourke/geometry/2circle/
		 */
		bool
		getIntersections(const Circle2D& other,
				PointSet2D<T>& intersectionPoints) const;
		
		/**
		 * \brief	Calculate intersection point(s)
		 * 
		 * \param[in]	other	Other line
		 * \param[out]	intersectionPoint	Intersection point
		 * 
		 * \see		Line2D::intersect()
		 */
		bool
		getIntersections(const Line2D<T>& line,
				PointSet2D<T>& intersectionPoints) const;
		
	protected:
		Vector2D<T> center;
		T radius;
		
	protected:
		friend class Line2D<T>;
		friend class LineSegment2D<T>;
	};
}

#include "line_2d.hpp"
#include "line_segment_2d.hpp"
#include "polygon_2d.hpp"

#include "circle_2d_impl.hpp"

#endif // XPCC__CIRCLE_2D_HPP
