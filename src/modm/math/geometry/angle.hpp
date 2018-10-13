/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_ANGLE_HPP
#define	MODM_ANGLE_HPP

#include <cmath>
#include <math.h>

// The circumference of a circle with diameter 1
#ifndef M_PI
#define M_PI  3.14159265358979323846
#endif

#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif

#ifndef M_PI_4
#define M_PI_4 0.78539816339744830962
#endif

#ifndef M_TWOPI
#define M_TWOPI (M_PI * 2.0)
#endif

#ifndef M_1_PI
#define M_1_PI 0.31830988618379067154
#endif

#ifndef M_2_PI
#define M_2_PI 0.63661977236758134308
#endif

// The square root of 2.
#ifndef M_SQRT2
#define M_SQRT2  1.41421356237309504880
#endif

namespace modm
{
	/// @ingroup modm_math_geometry
	static constexpr float
	toRadian(float angle)
	{
		return (angle * M_PI) / 180.f;
	}

	/// @ingroup modm_math_geometry
	static constexpr float
	toDegree(float angle)
	{
		return (angle * 180.f) / M_PI;
	}

	/**
	 * \brief	Collection of functions for handling of angles
	 *
	 * Angles are always represented by float values in the range
	 * from -Pi to Pi.
	 *
	 * @ingroup modm_math_geometry
	 */
	class Angle
	{
	public:
		typedef float Type;

		/**
		 * \brief	Normalize angle
		 *
		 * Normalize the given angle to [-Pi,Pi] by repeatedly
		 * adding/subtracting 2*Pi.
		 */
		static float
		normalize(float angle);

		/**
		 * \brief	Reverse the angle
		 *
		 * Reverse the angle and keep it normalized to [-Pi,Pi].
		 *
		 * Equivalent to:
		 * \code
		 * float angle = modm::Angle::normalize(angle + M_PI);
		 * \endcode
		 */
		static float
		reverse(float angle);

		/**
		 * \brief	Find a perpendicular angle
		 *
		 * \param	angle
		 * \param	cw	If cw is true the angle is rotated clockwise.
		 * 				Ohterwise the angle is rotated anti clockwise.
		 */

		static float
		perpendicular(float angle, const bool cw);

		static constexpr float
		toRadian(float angle)
		{
			return ::modm::toRadian(angle);
		}

		static constexpr float
		toDegree(float angle)
		{
			return ::modm::toDegree(angle);
		}
	};
}

#endif	// MODM_ANGLE_HPP
