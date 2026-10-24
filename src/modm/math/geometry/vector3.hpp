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

#ifndef MODM_VECTOR3_HPP
#define MODM_VECTOR3_HPP

#include <stdint.h>
#include "vector.hpp"

namespace modm
{
	/**
	 * \brief	Class for handling common vector operations (3D)
	 *
	 *	+ : addition of points
	 *	- : different of points
	 *	* : dot product or scalar multiplication
	 *	/ : scalar division
	 *	^ : cross product (determinant)
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Niklas Hauser
	 */
	template<typename T>
	class Vector<T, 3>
	{
	public:
		constexpr Vector();

		template<typename U>
		explicit constexpr Vector(const U *array);

		explicit constexpr Vector(T inVal);
		constexpr Vector(T inX, T inY, T inZ);

		constexpr Vector(const Vector<T, 1> &inX, const T &inY, const T &inZ);
		constexpr Vector(const T &inX, const Vector<T, 1> &inY, const T &inZ);
		constexpr Vector(const T &inX, const T &inY, const Vector<T, 1> &inZ);

		constexpr Vector(const Vector<T, 1> &inX, const T &inY, const Vector<T, 1> &inZ);
		constexpr Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const T &inZ);
		constexpr Vector(const T &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ);

		constexpr Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ);

		constexpr Vector(const Vector<T,2> &inXY, const T &inZ);
		constexpr Vector(const T &inX, const Vector<T, 2> &inYZ);

		constexpr Vector(const Vector<T, 2> &inXY, const Vector<T, 1> &inZ);
		constexpr Vector(const Vector<T, 1> &inX, const Vector<T, 2> &inYZ);

		template<typename U>
		constexpr Vector(const Vector<U, 3> &rhs);
		constexpr Vector(const Matrix<T, 3, 1> &rhs);


		constexpr void
		set(const T& x, const T& y, const T& z);


		constexpr void
		setX(const T& value);

		constexpr void
		setY(const T& value);

		constexpr void
		setZ(const T& value);


		constexpr const T&
		getX() const;

		constexpr const T&
		getY() const;

		constexpr const T&
		getZ() const;


		constexpr Vector& operator = (const Matrix<T, 3, 1> &rhs);

		constexpr bool operator == (const Vector &rhs) const;
		constexpr bool operator != (const Vector &rhs) const;
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
		constexpr Vector operator ^ (const Vector &rhs) const;
		constexpr Vector operator * (const T &rhs) const;
		constexpr Vector operator / (const T &rhs) const;

		constexpr Vector& operator += (const Vector &rhs);
		constexpr Vector& operator -= (const Vector &rhs);
		constexpr Vector& operator *= (const T &rhs);
		constexpr Vector& operator /= (const T &rhs);

		constexpr float getLength() const;
		constexpr float getLengthSquared() const;

		constexpr Vector scaled(float newLength) const;
		constexpr void scale(float newLength);

		constexpr Vector normalized() const;
		constexpr void normalize();

		constexpr Matrix<T, 3, 1>&
		asMatrix();

		constexpr const Matrix<T, 3, 1>&
		asMatrix() const;

		constexpr Matrix<T, 1, 3>&
		asTransposedMatrix();

		constexpr const Matrix<T, 1, 3>&
		asTransposedMatrix() const;

		constexpr bool hasNan() const;
		constexpr bool hasInf() const;

	#ifndef __DOXYGEN__
		IMPLEMENT_VECTOR_ACCESSOR2(x,x); IMPLEMENT_VECTOR_ACCESSOR2(x,y); IMPLEMENT_VECTOR_ACCESSOR2(x,z);
		IMPLEMENT_VECTOR_ACCESSOR2(y,x); IMPLEMENT_VECTOR_ACCESSOR2(y,y); IMPLEMENT_VECTOR_ACCESSOR2(y,z);
		IMPLEMENT_VECTOR_ACCESSOR2(z,x); IMPLEMENT_VECTOR_ACCESSOR2(z,y); IMPLEMENT_VECTOR_ACCESSOR2(z,z);

		IMPLEMENT_VECTOR_ACCESSOR3(x,x,x); IMPLEMENT_VECTOR_ACCESSOR3(x,x,y); IMPLEMENT_VECTOR_ACCESSOR3(x,x,z);
		IMPLEMENT_VECTOR_ACCESSOR3(x,y,x); IMPLEMENT_VECTOR_ACCESSOR3(x,y,y); IMPLEMENT_VECTOR_ACCESSOR3(x,y,z);
		IMPLEMENT_VECTOR_ACCESSOR3(x,z,x); IMPLEMENT_VECTOR_ACCESSOR3(x,z,y); IMPLEMENT_VECTOR_ACCESSOR3(x,z,z);
		IMPLEMENT_VECTOR_ACCESSOR3(y,x,x); IMPLEMENT_VECTOR_ACCESSOR3(y,x,y); IMPLEMENT_VECTOR_ACCESSOR3(y,x,z);
		IMPLEMENT_VECTOR_ACCESSOR3(y,y,x); IMPLEMENT_VECTOR_ACCESSOR3(y,y,y); IMPLEMENT_VECTOR_ACCESSOR3(y,y,z);
		IMPLEMENT_VECTOR_ACCESSOR3(y,z,x); IMPLEMENT_VECTOR_ACCESSOR3(y,z,y); IMPLEMENT_VECTOR_ACCESSOR3(y,z,z);
		IMPLEMENT_VECTOR_ACCESSOR3(z,x,x); IMPLEMENT_VECTOR_ACCESSOR3(z,x,y); IMPLEMENT_VECTOR_ACCESSOR3(z,x,z);
		IMPLEMENT_VECTOR_ACCESSOR3(z,y,x); IMPLEMENT_VECTOR_ACCESSOR3(z,y,y); IMPLEMENT_VECTOR_ACCESSOR3(z,y,z);
		IMPLEMENT_VECTOR_ACCESSOR3(z,z,x); IMPLEMENT_VECTOR_ACCESSOR3(z,z,y); IMPLEMENT_VECTOR_ACCESSOR3(z,z,z);

		IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y); IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y); IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,x); IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,y); IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y); IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y); IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,x); IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,y); IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,x); IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,y); IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,x); IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,y); IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,x); IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,y); IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,z);

		IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y); IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y); IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,x); IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,y); IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y); IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y); IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,x); IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,y); IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,x); IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,y); IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,x); IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,y); IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,x); IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,y); IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,z);

		IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,x); IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,y); IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,x); IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,y); IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,x); IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,y); IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,x); IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,y); IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,x); IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,y); IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,x); IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,y); IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,x); IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,y); IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,x); IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,y); IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,z);
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,x); IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,y); IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,z);
	#endif

	public:
		T x;
		T y;
		T z;
	};

	template<typename U, typename T>
	constexpr static inline Vector<T, 3> operator * (const U &lhs, const Vector<T, 3> &rhs)
	{
		return rhs * lhs;
	}

	template<typename T, typename U>
	constexpr static inline Vector<U, 3> operator * (const Matrix<T, 3, 3> &lhs, const Vector<U, 3> &rhs)
	{
		return lhs * rhs.asMatrix();
	}


	using Vector3f = Vector<float, 3>;
	using Vector3i = Vector<int16_t, 3>;
	using Vector3u = Vector<uint16_t, 3>;
	using Vector3li = Vector<int32_t, 3>;
	using Vector3lu = Vector<uint32_t, 3>;
}

#include "vector3_impl.hpp"

#endif // MODM_VECTOR3_HPP
