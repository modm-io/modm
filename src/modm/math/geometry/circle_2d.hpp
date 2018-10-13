/*
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

#ifndef MODM_CIRCLE_2D_HPP
#define MODM_CIRCLE_2D_HPP

#include <cmath>
#include "geometric_traits.hpp"

#include "vector.hpp"
#include "point_set_2d.hpp"

namespace modm
{
	// forward declarations
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
	 * \ingroup	modm_math_geometry
	 */
	template <typename T>
	class Circle2D
	{
		typedef typename GeometricTraits<T>::WideType WideType;
		typedef typename GeometricTraits<T>::FloatType FloatType;

	public:
		Circle2D();

		Circle2D(const Vector<T, 2>& center, T radius);

		inline const Vector<T, 2>&
		getCenter() const;

		inline void
		setCenter(const Vector<T, 2>& point);

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
		 * \param[in]	other			Other Circle
		 * \param[out]	intersections	Intersection points
		 *
		 * \see		http://local.wasp.uwa.edu.au/~pbourke/geometry/2circle/
		 */
		bool
		getIntersections(const Circle2D& other,
				PointSet2D<T>& intersections) const;

		/**
		 * \brief	Calculate intersection point(s)
		 *
		 * \param[in]	line			Other line
		 * \param[out]	intersections	Intersection points
		 *
		 * \see		Line2D::intersect()
		 */
		bool
		getIntersections(const Line2D<T>& line,
				PointSet2D<T>& intersections) const;

		bool
		getIntersections(const LineSegment2D<T>& line,
				PointSet2D<T>& intersections) const;

	protected:
		Vector<T, 2> center;
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

#endif // MODM_CIRCLE_2D_HPP
