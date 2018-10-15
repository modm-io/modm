/*
 * Copyright (c) 2009-2011, 2013, Fabian Greif
 * Copyright (c) 2012, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_RAY_2D_HPP
#define MODM_RAY_2D_HPP

#include "geometric_traits.hpp"

#include "vector.hpp"
#include "point_set_2d.hpp"

namespace modm
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
	 * \ingroup	modm_math_geometry
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
		ccw(const modm::Vector<T, 2>& point);

		bool
		operator == (const Ray2D &other) const;

		bool
		operator != (const Ray2D &other) const;

	protected:
		modm::Vector<T, 2> basePoint;
		modm::Vector<T, 2> direction;
	};
}

#include "circle_2d.hpp"
#include "polygon_2d.hpp"
#include "line_segment_2d.hpp"

#include "ray_2d_impl.hpp"

#endif // MODM_RAY_2D_HPP
