/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_QUATERNION_HPP
#define MODM_QUATERNION_HPP

#include <cmath>
#include <stdint.h>

namespace modm
{
	// forward declaration
	template<class T, uint8_t N>
	class Vector;

	template<class T, uint8_t ROWS, uint8_t COLUMNS>
	class Matrix;

	/**
	 * \brief	Quaternion
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/quat.html">Homepage</a>
	 *
	 * \author	Fabian Greif
	 * \ingroup	modm_math_geometry
	 */
	template<class T>
	class Quaternion
	{
	public:
		constexpr Quaternion();
		template<typename U>
		explicit constexpr Quaternion(const U *array);
		constexpr Quaternion(T inW, T inX, T inY, T inZ);
		constexpr Quaternion(const Vector<T, 3> &axis, T angle);
		constexpr Quaternion(const Matrix<T, 4, 1> &rhs);

		constexpr Quaternion(const Quaternion &rhs);

		constexpr Quaternion& operator = (const Matrix<T, 4, 1> &rhs);
		constexpr Quaternion& operator = (const Quaternion &rhs);

		constexpr bool operator == (const Quaternion &rhs) const = default;
		constexpr bool operator < (const Quaternion &rhs) const;
		constexpr bool operator <= (const Quaternion &rhs) const;
		constexpr bool operator > (const Quaternion &rhs) const;
		constexpr bool operator >= (const Quaternion &rhs) const;
		constexpr const T& operator [] (int index) const;

		constexpr T*
		ptr();

		constexpr const T*
		ptr() const;

		constexpr Quaternion operator - ();
		constexpr Quaternion operator + (const Quaternion &rhs) const;
		constexpr Quaternion operator - (const Quaternion &rhs) const;
		constexpr Quaternion operator * (const Quaternion &rhs) const;

		constexpr Quaternion operator * (const T &rhs) const;
		constexpr Quaternion operator / (const T &rhs) const;
		constexpr Quaternion& operator += (const Quaternion &rhs);
		constexpr Quaternion& operator -= (const Quaternion &rhs);
		constexpr Quaternion& operator *= (const T &rhs);
		constexpr Quaternion& operator /= (const T &rhs);

		constexpr float getLength() const;
		constexpr float getLengthSquared() const;

		constexpr Quaternion& scale(float newLength);

		/** Rescale to length = 1 */
		constexpr Quaternion& normalize();
		constexpr Quaternion& conjugate();

		constexpr Quaternion scaled(float newLength) const;

		/** Return a new quaternion with the same direction by the length 1 */
		constexpr Quaternion normalized() const;
		constexpr Quaternion conjugated() const;

		Quaternion unitInverse();	//	assumes we have a unit quaternion
		constexpr Quaternion inverse();

		constexpr void to4x4Matrix(Matrix<T, 4, 4> *outMatrix);
		constexpr void to3x3Matrix(Matrix<T, 3, 3> *outMatrix);

	public:
		T w;
		T x;
		T y;
		T z;
	};

	template<class T>
	constexpr Quaternion<T>
	operator *(const T &lhs, const Quaternion<T> &rhs);
}

#include "quaternion_impl.hpp"

#endif	// MODM_QUATERNION_HPP
