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
		Vector();

		template<typename U>
		explicit Vector(const U *array);

		explicit Vector(T inVal);
		Vector(T inX, T inY, T inZ);

		Vector(const Vector<T, 1> &inX, const T &inY, const T &inZ);
		Vector(const T &inX, const Vector<T, 1> &inY, const T &inZ);
		Vector(const T &inX, const T &inY, const Vector<T, 1> &inZ);

		Vector(const Vector<T, 1> &inX, const T &inY, const Vector<T, 1> &inZ);
		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const T &inZ);
		Vector(const T &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ);

		Vector(const Vector<T, 1> &inX, const Vector<T, 1> &inY, const Vector<T, 1> &inZ);

		Vector(const Vector<T,2> &inXY, const T &inZ);
		Vector(const T &inX, const Vector<T, 2> &inYZ);

		Vector(const Vector<T, 2> &inXY, const Vector<T, 1> &inZ);
		Vector(const Vector<T, 1> &inX, const Vector<T, 2> &inYZ);

		template<typename U>
		Vector(const Vector<U, 3> &rhs);
		Vector(const Matrix<T, 3, 1> &rhs);


		inline void
		set(const T& x, const T& y, const T& z);


		inline void
		setX(const T& value);

		inline void
		setY(const T& value);

		inline void
		setZ(const T& value);


		inline const T&
		getX() const;

		inline const T&
		getY() const;

		inline const T&
		getZ() const;


		Vector& operator = (const Matrix<T, 3, 1> &rhs);

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
		Vector operator ^ (const Vector &rhs) const;
		Vector operator * (const T &rhs) const;
		Vector operator / (const T &rhs) const;

		Vector& operator += (const Vector &rhs);
		Vector& operator -= (const Vector &rhs);
		Vector& operator *= (const T &rhs);
		Vector& operator /= (const T &rhs);

		float getLength() const;
		float getLengthSquared() const;

		Vector scaled(float newLength) const;
		void scale(float newLength);

		Vector normalized() const;
		void normalize();

		Matrix<T, 3, 1>&
		asMatrix();

		const Matrix<T, 3, 1>&
		asMatrix() const;

		Matrix<T, 1, 3>&
		asTransposedMatrix();

		const Matrix<T, 1, 3>&
		asTransposedMatrix() const;

		bool hasNan() const;
		bool hasInf() const;

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
	static inline Vector<T, 3> operator * (const U &lhs, const Vector<T, 3> &rhs)
	{
		return rhs * lhs;
	}

	template<typename T, typename U>
	static inline Vector<U, 3> operator * (const Matrix<T, 3, 3> &lhs, const Vector<U, 3> &rhs)
	{
		return lhs * rhs.asMatrix();
	}


	typedef Vector<float, 3> 	Vector3f;
	typedef Vector<int16_t, 3>	Vector3i;
	typedef Vector<uint16_t, 3>	Vector3u;
}

#include "vector3_impl.hpp"

#endif // MODM_VECTOR3_HPP
