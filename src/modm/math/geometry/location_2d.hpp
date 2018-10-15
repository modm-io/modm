/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, Niklas Hauser
 * Copyright (c) 2012, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_LOCATION_2D_HPP
#define	MODM_LOCATION_2D_HPP

#include <cmath>

#include <modm/io/iostream.hpp>

#include "angle.hpp"
#include "vector.hpp"

namespace modm
{
	/**
	 * \brief	Location in a 2D coordinate system
	 *
	 * This class is primarily used to describe the location of a robot
	 * on the field. The robot has a position (x- and y-coordinate) and a
	 * orientation (absolute angle in the range (-pi, pi)).
	 *
	 * \ingroup	modm_math_geometry
	 */
	template <typename T = int16_t>
	class Location2D
	{
	public:
		Location2D();

		Location2D(const Vector<T, 2>& position, const float& orientation);

		Location2D(const T& x, const T& y, const float& orientation);

		inline const Vector<T, 2>&
		getPosition() const;

		inline const T&
		getX() const;

		inline const T&
		getY() const;

		void
		setPosition(const Vector<T, 2>& point);

		void
		setPosition(const T& x, const T& y);

		inline float
		getOrientation() const;

		void
		setOrientation(const float& phi);

		/// Add a position increment
		void
		move(const Location2D& diff);

		void
		move(const Vector<T, 2>& diff);

		/**
		 * \brief	Add a increment only in x-direction
		 *
		 * Our robots mostly use a differential drive with two driven wheels
		 * side by side, allowing the robot to move only in the drive direction
		 * (x-direction in the local coordinate system of the robot) and
		 * rotate. A movement perpendicular to the drive direction is
		 * impossible without an external force.
		 *
		 * To estimate the position of the robot over time, we use odometry.
		 * Therefore it is necessary to add a lot small increments of
		 * movement over time.
		 * Because the y-component will always be zero, we created this
		 * method, which avoids unnecessary computations for the y-component
		 * and is therefore faster the the universal move-method.
		 *
		 * \param	x		movement in x-direction
		 * \param	phi		rotation
		 */
		void
		move(T x, float phi);

		/// TODO
		Vector<T, 2>
		translated(const Vector<T, 2>& vector) const;

		/// Convert between Location-objects with different base-types
		template <typename U>
		Location2D<U>
		convert() const;

		bool
		operator == (const Location2D &other) const;

		bool
		operator != (const Location2D &other) const;

	private:
		template <typename U>
		friend IOStream&
		operator <<( IOStream&, const Location2D<U>&);

		Vector<T, 2> position;
		float orientation;
	};

	// ------------------------------------------------------------------------
	// Global functions
	// ------------------------------------------------------------------------
	/**
	 * \brief	Stream operator to \b modm::Location<T>
	 *
	 * \ingroup	modm_math_geometry
	 */
	template<typename T>
	IOStream&
	operator << (IOStream& os, const Location2D<T>& l);

	// ------------------------------------------------------------------------
	// Declaration of specialized methods
	// ------------------------------------------------------------------------
	/*template<>
	bool
	Location2D<float>::operator == (const Location2D &other) const;

	template<>
	bool
	Location2D<double>::operator == (const Location2D &other) const;*/
}

#include "location_2d_impl.hpp"

#endif	// MODM_LOCATION_2D_HPP
