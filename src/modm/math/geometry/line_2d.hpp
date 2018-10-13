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

#ifndef MODM_LINE_2D_HPP
#define MODM_LINE_2D_HPP

#include "vector.hpp"
#include "point_set_2d.hpp"

#include "geometric_traits.hpp"

namespace modm
{
	// forward declaration
	template <typename T>
	class Circle2D;

	/**
	 * \brief	Line
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_math_geometry
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

#endif // MODM_LINE_2D_HPP
