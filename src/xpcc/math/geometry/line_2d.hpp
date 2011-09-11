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
#define XPCC__LINE_2D_HPP

#include "vector.hpp"
#include "point_set_2d.hpp"

#include "geometric_traits.hpp"

namespace xpcc
{
	// forward declaration
	template <typename T>
	class Circle2D;
	
	/**
	 * \brief	Line
	 * 
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template <typename T>
	class Line2D
	{
	public:
		typedef typename GeometricTraits<T>::WideType WideType;
		typedef typename GeometricTraits<T>::FloatType FloatType;
		
	public:
		/**
		 * \brief	Default-Constructor
		 */
		Line2D();
		
		/**
		 * \brief	Construct a line
		 * 
		 * \param	point			a point on the line
		 * \param	directionVector	direction vector, the length doesn't matter
		 */
		Line2D(const Vector<T, 2>& point, const Vector<T, 2>& directionVector);
		
		
		inline void
		setPoint(const Vector<T, 2>& point);
		
		inline const Vector<T, 2>&
		getPoint() const;
		
		inline void
		setDirectionVector(const Vector<T, 2>& vector);
		
		inline const Vector<T, 2>&
		getDirectionVector() const;
		
		inline void
		set(const Vector<T, 2>& point, const Vector<T, 2>& directionVector);
		
		/// Shortest distance to a point
		T
		getDistanceTo(const Vector<T, 2>& point) const;
		
		/**
		 * \brief	Calculate intersection point(s)
		 * 
		 * \param[in]	other	Other line
		 * \param[out]	intersections	Intersection point
		 */
		bool
		getIntersections(const Line2D& other,
				PointSet2D<T>& intersections) const;
		
		/**
		 * \brief	Calculate intersection point(s)
		 * 
		 * \param[in]	circle	A circle
		 * \param[out]	intersections	Intersection points
		 * 
		 * \see		http://local.wasp.uwa.edu.au/~pbourke/geometry/sphereline/
		 */
		bool
		getIntersections(const Circle2D<T>& circle,
				PointSet2D<T>& intersections) const;
		
	protected:
		Vector<T, 2> point;
		Vector<T, 2> directionVector;
	};
}

#include "circle_2d.hpp"

#include "line_2d_impl.hpp"

#endif // XPCC__LINE_2D_HPP
