/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2012, Fabian Greif
 * Copyright (c) 2011-2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VECTOR1_HPP
#define MODM_VECTOR1_HPP

#include <cmath>
#include <cstdlib>
#include <stdint.h>

#include "vector.hpp"

namespace modm
{
	/**
	 * \brief	Class for handling common vector operations (1D)
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Niklas Hauser
	 */
	template<typename T>
	class Vector<T, 1>
	{
	public:
		Vector();
		Vector(T inX);
		Vector(const Matrix<T, 1, 1> &rhs);

		inline void
		set(const T& x);

		inline void
		setX(const T& value);

		inline const T&
		getX() const;

		Vector& operator = (const Matrix<T, 1, 1> &rhs);

		bool operator == (const Vector &rhs) const;
		bool operator != (const Vector &rhs) const;
		bool operator < (const Vector &rhs) const;
		bool operator <= (const Vector &rhs) const;
		bool operator > (const Vector &rhs) const;
		bool operator >= (const Vector &rhs) const;

		const T& operator [] (uint8_t index) const;
		T& operator [] (uint8_t index);
		T* ptr();
		const T* ptr() const;

		Vector operator - () const;
		Vector operator + (const Vector &rhs) const;
		Vector operator - (const Vector &rhs) const;
		T operator * (const Vector &rhs) const;
		Vector operator * (const T &rhs) const;
		Vector operator / (const T &rhs) const;

		Vector& operator += (const Vector &rhs);
		Vector& operator -= (const Vector &rhs);
		Vector& operator *= (const T &rhs);
		Vector& operator /= (const T &rhs);

		T getLength() const;
		T getLengthSquared() const;

		Matrix<T, 1, 1>&
		asMatrix();

		const Matrix<T, 1, 1>&
		asMatrix() const;

		bool hasNan() const;
		bool hasInf() const;

	public:
		T x;

	public:
	#ifndef __DOXYGEN__
		IMPLEMENT_VECTOR_ACCESSOR2(x,x)
		IMPLEMENT_VECTOR_ACCESSOR3(x,x,x)
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x)
	#endif
	};

	template<typename U, typename T>
	static inline Vector<T, 1> operator * (const U &lhs, const Vector<T, 1> &rhs)
	{
		return rhs * lhs;
	}

	typedef Vector<float, 1> 	Vector1f;
	typedef Vector<int16_t, 1> 	Vector1i;
}

#include "vector1_impl.hpp"

#endif // MODM_VECTOR1_HPP
