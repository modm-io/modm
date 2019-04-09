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
		Quaternion();
		template<typename U>
		explicit Quaternion(const U *array);
		Quaternion(T inW, T inX, T inY, T inZ);
		Quaternion(const Vector<T, 3> &axis, T angle);
		Quaternion(const Matrix<T, 4, 1> &rhs);

		Quaternion(const Quaternion &rhs);

		Quaternion& operator = (const Matrix<T, 4, 1> &rhs);
		Quaternion& operator = (const Quaternion &rhs);

		bool operator == (const Quaternion &rhs) const;
		bool operator != (const Quaternion &rhs) const;
		bool operator < (const Quaternion &rhs) const;
		bool operator <= (const Quaternion &rhs) const;
		bool operator > (const Quaternion &rhs) const;
		bool operator >= (const Quaternion &rhs) const;
		const T& operator [] (int index) const;

		T*
		ptr();

		const T*
		ptr() const;

		Quaternion operator - ();
		Quaternion operator + (const Quaternion &rhs) const;
		Quaternion operator - (const Quaternion &rhs) const;
		Quaternion operator * (const Quaternion &rhs) const;

		Quaternion operator * (const T &rhs) const;
		Quaternion operator / (const T &rhs) const;
		Quaternion& operator += (const Quaternion &rhs);
		Quaternion& operator -= (const Quaternion &rhs);
		Quaternion& operator *= (const T &rhs);
		Quaternion& operator /= (const T &rhs);

		float getLength() const;
		float getLengthSquared() const;

		Quaternion& scale(float newLength);

		/** Rescale to length = 1 */
		Quaternion& normalize();
		Quaternion& conjugate();

		Quaternion scaled(float newLength) const;

		/** Return a new quaternion with the same direction by the length 1 */
		Quaternion normalized() const;
		Quaternion conjugated() const;

		Quaternion unitInverse();	//	assumes we have a unit quaternion
		Quaternion inverse();

		void to4x4Matrix(Matrix<T, 4, 4> *outMatrix);
		void to3x3Matrix(Matrix<T, 3, 3> *outMatrix);

	public:
		T w;
		T x;
		T y;
		T z;
	};

	template<class T>
	Quaternion<T>
	operator * (const T &lhs, const Quaternion<T> &rhs);
}

#include "quaternion_impl.hpp"

#endif	// MODM_QUATERNION_HPP
