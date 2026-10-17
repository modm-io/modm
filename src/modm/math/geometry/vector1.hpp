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
		constexpr Vector();
		constexpr Vector(T inX);
		constexpr Vector(const Matrix<T, 1, 1> &rhs);

		constexpr void
		set(const T& x);

		constexpr void
		setX(const T& value);

		constexpr const T&
		getX() const;

		constexpr Vector& operator = (const Matrix<T, 1, 1> &rhs);

		constexpr bool operator == (const Vector &rhs) const = default;
		constexpr bool operator < (const Vector &rhs) const;
		constexpr bool operator <= (const Vector &rhs) const;
		constexpr bool operator > (const Vector &rhs) const;
		constexpr bool operator >= (const Vector &rhs) const;

		constexpr const T& operator [] (uint8_t index) const;
		constexpr T& operator [] (uint8_t index);
		constexpr T* ptr();
		constexpr const T* ptr() const;

		constexpr Vector operator - () const;
		constexpr Vector operator + (const Vector &rhs) const;
		constexpr Vector operator - (const Vector &rhs) const;
		constexpr T operator * (const Vector &rhs) const;
		constexpr Vector operator * (const T &rhs) const;
		constexpr Vector operator / (const T &rhs) const;

		constexpr Vector& operator += (const Vector &rhs);
		constexpr Vector& operator -= (const Vector &rhs);
		constexpr Vector& operator *= (const T &rhs);
		constexpr Vector& operator /= (const T &rhs);

		constexpr T getLength() const;
		constexpr T getLengthSquared() const;

		constexpr Matrix<T, 1, 1>&
		asMatrix();

		constexpr const Matrix<T, 1, 1>&
		asMatrix() const;

		constexpr bool hasNan() const;
		constexpr bool hasInf() const;

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
	constexpr static inline Vector<T, 1> operator * (const U &lhs, const Vector<T, 1> &rhs)
	{
		return rhs * lhs;
	}

	using Vector1f = Vector<float, 1>;
	using Vector1i = Vector<int16_t, 1>;
}

#include "vector1_impl.hpp"

#endif // MODM_VECTOR1_HPP
