/*
 * Copyright (c) 2009-2011, Fabian Greif
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

#ifndef MODM_POINT_SET_2D_HPP
#define MODM_POINT_SET_2D_HPP

#include <vector>
#include "vector.hpp"

namespace modm
{
	/**
	 * \brief	Point set
	 *
	 * Collection of points, represented by their corresponding vectors.
	 * Used for example to hold the result of a intersection-operation.
	 *
	 * Based on the std::vector class, therefore grows automatically
	 * if more space than currently allocated is needed. But because this
	 * is an expensive operation it should be avoid if possible.
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_math_geometry
	 */
	template <typename T>
	class PointSet2D
	{
	public:
		using SizeType = std::size_t;
		using PointType = Vector<T, 2>;

	public:
		/**
		 * \brief	Constructs a set capable of holding n points (default = 2)
		 */
		constexpr PointSet2D(SizeType n = 2);

		constexpr PointSet2D(std::initializer_list<PointType> init);

		constexpr PointSet2D(const PointSet2D& other);

		PointSet2D&
		operator = (const PointSet2D& other);

		/// Number of points contained in the set
		inline SizeType
		getNumberOfPoints() const;


		inline void
		append(const PointType& point);

		inline PointType&
		operator [](SizeType index);

		inline const PointType&
		operator [](SizeType index) const;

		/**
		 * \brief	Remove all points
		 */
		inline void
		removeAll();

	public:
		using iterator = typename std::vector< PointType >::iterator;
		using const_iterator = typename std::vector< PointType >::const_iterator;

		inline iterator
		begin();

		inline const_iterator
		begin() const;

		inline iterator
		end();

		inline const_iterator
		end() const;

	protected:
		std::vector< PointType > points;
	};
}

#include "point_set_2d_impl.hpp"

#endif // MODM_POINT_SET_2D_HPP
