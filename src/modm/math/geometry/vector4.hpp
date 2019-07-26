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

#ifndef MODM_VECTOR4_HPP
#define MODM_VECTOR4_HPP

#include <stdint.h>
#include "vector.hpp"

namespace modm
{
	/**
	 * \brief	Class for handling common vector operations (4D)
	 *
	 *	+ : addition of points
	 *	- : different of points
	 *	* : dot product or scalar multiplication
	 *	/ : scalar division
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Niklas Hauser
	 */
	template<typename T>
	class Vector<T, 4>
	{
	public:
		Vector();

		explicit Vector(T inVal);
		Vector(T inX, T inY, T inZ, T inW);

		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ, const Vector<T, 1> &inW);
		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ, const T &inW);
		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const T &inZ, const T &inW);
		Vector(const Vector<T, 1> &inX, const T &inY, const Vector<T, 1> &inZ, const T &inW);
		Vector(const T &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ, const T &inW);
		Vector(const Vector<T, 1> &inX, const T &inY, const T &inZ, const T &inW);
		Vector(const T &inX, const Vector<T, 1> &inY, const T &inZ, const T &inW);
		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const T &inZ, const Vector<T, 1> &inW);
		Vector(const Vector<T, 1> &inX, const T &inY, const T &inZ, const Vector<T, 1> &inW);
		Vector(const T &inX, const Vector<T, 1> &inY, const T &inZ, const Vector<T, 1> &inW);
		Vector(const T &inX, const T &inY, const T &inZ, const Vector<T, 1> &inW);
		Vector(const Vector<T, 1> &inX, const T &inY, const Vector<T, 1> &inZ, const Vector<T, 1> &inW);
		Vector(const T &inX, const T &inY, const Vector<T, 1> &inZ, const Vector<T, 1> &inW);
		Vector(const T &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ, const Vector<T, 1> &inW);

		Vector(const Vector<T, 2> &inXY, const Vector<T, 1> &inZ, const Vector<T, 1> &inW);
		Vector(const Vector<T, 2> &inXY, const Vector<T, 1> &inZ, const T &inW);
		Vector(const Vector<T, 2> &inXY, const T &inZ, const T &inW);
		Vector(const Vector<T, 2> &inXY, const T &inZ, const Vector<T, 1> &inW);

		Vector(const Vector<T, 1> &inX, const Vector<T, 2> &inYZ, const Vector<T, 1> &inW);
		Vector(const Vector<T, 1> &inX, const Vector<T, 2> &inYZ, const T &inW);
		Vector(const T &inX, const Vector<T, 2> &inYZ, const T &inW);
		Vector(const T &inX, const Vector<T, 2> &inYZ, const Vector<T, 1> &inW);

		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const Vector<T, 2> &inZW);
		Vector(const Vector<T, 1> &inX, const T &inY, const Vector<T, 2> &inZW);
		Vector(const T &inX, const T &inY, const Vector<T, 2> &inZW);
		Vector(const T &inX, const Vector<T, 1> &inY, const Vector<T, 2> &inZW);

		Vector(const Vector<T, 2> &inXY, const Vector<T, 2> &inZW);

		Vector(const Vector<T, 3> &inXYZ, const Vector<T, 1> &inW);
		Vector(const Vector<T, 3> &inXYZ, const T &inW);

		Vector(const Vector<T, 1> &inX, const Vector<T, 3> &inYZW);
		Vector(const T &inX, const Vector<T, 3> &inYZW);

		Vector(const Matrix<T, 4, 1> &rhs);


		inline void
		set(const T& x, const T& y, const T& z, const T& w);


		inline void
		setX(const T& value);

		inline void
		setY(const T& value);

		inline void
		setZ(const T& value);

		inline void
		setW(const T& value);


		inline const T&
		getX() const;

		inline const T&
		getY() const;

		inline const T&
		getZ() const;

		inline const T&
		getW() const;


		Vector& operator = (const Matrix<T, 4, 1> &rhs);

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

		float getLength() const;
		float getLengthSquared() const;

		void scale(float newLength);
		Vector scaled(float newLength) const;

		void normalize();
		Vector normalized() const;

		Matrix<T, 4, 1>&
		asMatrix();

		const Matrix<T, 4, 1>&
		asMatrix() const;

		Matrix<T, 1, 4>&
		asTransposedMatrix();

		const Matrix<T, 1, 4>&
		asTransposedMatrix() const;

	#ifndef __DOXYGEN__
		IMPLEMENT_VECTOR_ACCESSOR2(x,x)	IMPLEMENT_VECTOR_ACCESSOR2(x,y)	IMPLEMENT_VECTOR_ACCESSOR2(x,z)	IMPLEMENT_VECTOR_ACCESSOR2(x,w)
		IMPLEMENT_VECTOR_ACCESSOR2(y,x)	IMPLEMENT_VECTOR_ACCESSOR2(y,y)	IMPLEMENT_VECTOR_ACCESSOR2(y,z)	IMPLEMENT_VECTOR_ACCESSOR2(y,w)
		IMPLEMENT_VECTOR_ACCESSOR2(z,x)	IMPLEMENT_VECTOR_ACCESSOR2(z,y)	IMPLEMENT_VECTOR_ACCESSOR2(z,z)	IMPLEMENT_VECTOR_ACCESSOR2(z,w)
		IMPLEMENT_VECTOR_ACCESSOR2(w,x)	IMPLEMENT_VECTOR_ACCESSOR2(w,y)	IMPLEMENT_VECTOR_ACCESSOR2(w,z)	IMPLEMENT_VECTOR_ACCESSOR2(w,w)

		IMPLEMENT_VECTOR_ACCESSOR3(x,x,x) IMPLEMENT_VECTOR_ACCESSOR3(x,x,y) IMPLEMENT_VECTOR_ACCESSOR3(x,x,z) IMPLEMENT_VECTOR_ACCESSOR3(x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(x,y,x) IMPLEMENT_VECTOR_ACCESSOR3(x,y,y) IMPLEMENT_VECTOR_ACCESSOR3(x,y,z) IMPLEMENT_VECTOR_ACCESSOR3(x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(x,z,x) IMPLEMENT_VECTOR_ACCESSOR3(x,z,y) IMPLEMENT_VECTOR_ACCESSOR3(x,z,z) IMPLEMENT_VECTOR_ACCESSOR3(x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,x,x) IMPLEMENT_VECTOR_ACCESSOR3(y,x,y) IMPLEMENT_VECTOR_ACCESSOR3(y,x,z) IMPLEMENT_VECTOR_ACCESSOR3(y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,y,x) IMPLEMENT_VECTOR_ACCESSOR3(y,y,y) IMPLEMENT_VECTOR_ACCESSOR3(y,y,z) IMPLEMENT_VECTOR_ACCESSOR3(y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(y,z,x) IMPLEMENT_VECTOR_ACCESSOR3(y,z,y) IMPLEMENT_VECTOR_ACCESSOR3(y,z,z) IMPLEMENT_VECTOR_ACCESSOR3(y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,x,x) IMPLEMENT_VECTOR_ACCESSOR3(z,x,y) IMPLEMENT_VECTOR_ACCESSOR3(z,x,z) IMPLEMENT_VECTOR_ACCESSOR3(z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,y,x) IMPLEMENT_VECTOR_ACCESSOR3(z,y,y) IMPLEMENT_VECTOR_ACCESSOR3(z,y,z) IMPLEMENT_VECTOR_ACCESSOR3(z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(z,z,x) IMPLEMENT_VECTOR_ACCESSOR3(z,z,y) IMPLEMENT_VECTOR_ACCESSOR3(z,z,z) IMPLEMENT_VECTOR_ACCESSOR3(z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,x,x) IMPLEMENT_VECTOR_ACCESSOR3(w,x,y) IMPLEMENT_VECTOR_ACCESSOR3(w,x,z) IMPLEMENT_VECTOR_ACCESSOR3(w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,y,x) IMPLEMENT_VECTOR_ACCESSOR3(w,y,y) IMPLEMENT_VECTOR_ACCESSOR3(w,y,z) IMPLEMENT_VECTOR_ACCESSOR3(w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR3(w,z,x) IMPLEMENT_VECTOR_ACCESSOR3(w,z,y) IMPLEMENT_VECTOR_ACCESSOR3(w,z,z) IMPLEMENT_VECTOR_ACCESSOR3(w,z,w)

		IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(x,w,z,w)

		IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(y,w,z,w)

		IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(z,w,z,w)

		IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,x,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,y,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,z,z,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,x,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,y,w)
		IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,x) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,y) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,z) IMPLEMENT_VECTOR_ACCESSOR4(w,w,z,w)
	#endif

	public:
		T x;
		T y;
		T z;
		T w;
	};

	template<typename U, typename T>
	static inline Vector<T,4> operator * (const U &lhs, const Vector<T,4> &rhs)
	{
		return rhs * lhs;
	}

	template<typename T, typename U>
	static inline Vector<U,4> operator * (const Matrix<T, 4, 4> &lhs, const Vector<U,4> &rhs)
	{
		return lhs * rhs.asTMatrix();
	}

	typedef Vector<float, 4> 	Vector4f;
	typedef Vector<int16_t, 4>	Vector4i;
	typedef Vector<uint16_t, 4>	Vector4u;
}

#include "vector4_impl.hpp"

#endif // MODM_VECTOR4_HPP
