/*
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
#define MODM_POLYGON_2D_HPP

#include "point_set_2d.hpp"
#include "vector2.hpp"

namespace modm
{
	// forward declaration
	template <typename T>
	class Circle2D;

	template <typename T>
	class LineSegment2D;

	template <typename T>
	class Ray2D;

	/**
	 * \brief	Polygon
	 *
	 * The Polygon class provides a vector of points. The polygon is
	 * implicit closed, which means the first and the last point are connected.
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_math_geometry
	 */
	template <typename T>
	class Polygon2D : public PointSet2D<T>
	{
		using SizeType = std::size_t;
		using PointType = typename PointSet2D<T>::PointType;
	public:
		/**
		 * \brief	Constructs a polygon capable of holding n points
		 */
		Polygon2D(SizeType n);

		Polygon2D(std::initializer_list<PointType> init);

		Polygon2D(const Polygon2D& other);

		Polygon2D&
		operator = (const Polygon2D& other);

		/// append a point to the polygon
		Polygon2D&
		operator << (const PointType& point);

		/**
		 * \brief	Check if a intersection exists
		 *
		 * \todo	Currently a brute force approach is used here,
		 * 			needs to be optimized
		 */
		bool
		intersects(const Polygon2D& other) const;

		/// Check if a intersection exists
		bool
		intersects(const Circle2D<T>& circle) const;

		/// Check if a intersection exists
		bool
		intersects(const LineSegment2D<T>& segment) const;

		/// Check if a intersection exists
		bool
		intersects(const Ray2D<T>& segment) const;

		/**
		 * \brief	Calculate the intersection point(s)
		 */
		bool
		getIntersections(const LineSegment2D<T>& segment,
				PointSet2D<T>& intersectionPoints) const;

		/**
		 * Check if the point is contained inside the area of the polygon.
		 *
		 * The borders of the polygon are included in the area of the polygon.
		 *
		 * @warning Only works for convex polygons!
		 *
		 * @return  `true` if point is contained in the area, `false` if
		 *          it is outside.
		 */
		bool
		isInside(const PointType& point);
	};
}

#include "circle_2d.hpp"
#include "line_segment_2d.hpp"
#include "ray_2d.hpp"

#include "polygon_2d_impl.hpp"

#endif // MODM_POLYGON_2D_HPP
