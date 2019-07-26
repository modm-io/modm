/*
 * Copyright (c) 2011-2012, Fabian Greif
 * Copyright (c) 2012, Georgi Grinshpun
 * Copyright (c) 2012, Martin Rosekeit
 * Copyright (c) 2012, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_VECTOR_HPP
#define MODM_VECTOR_HPP

#include <cmath>
#include <stdint.h>

#include <modm/math/matrix.hpp>
#include <modm/math/utils/arithmetic_traits.hpp>

namespace modm
{
	// forward declaration
	template<typename T, uint8_t W, uint8_t H> class Matrix;

	/**
	 * \brief	Class for handling common point operations
	 *
	 * Basic data type of all geometric operations. Used to represent vectors
	 * as well as particular points in the coordinate system.
	 *
	 * \section	point_vector	Point vs. vector
	 *
	 * In geometry, it is often convenient to use vector arithmetic to
	 * represent points.
	 *
	 * A vector, by its definition, has no fixed starting point, but if we
	 * imagine the starting point of a vector to be the origin, then the
	 * endpoint of the vector represents a particular point.
	 *
	 * In this manner, every vector can be said to identify a unique point,
	 * which is the endpoint of the vector when its starting point is the
	 * origin.
	 *
	 * Therefore there isn't a Point-class, but only a Vector class.
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 *
	 * \ingroup	modm_math_geometry
	 * \author	Niklas Hauser
	 */
	template<typename T, uint8_t N>
	class Vector
	{
	public:
		Vector();
		Vector(const T *ptData);

		Vector(const Matrix<T, N, 1> &rhs);
		Vector& operator = (const Matrix<T, N, 1> &rhs);

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

		Vector operator + (const Vector &rhs) const;
		Vector operator - (const Vector &rhs) const;
		T operator * (const Vector &rhs) const;
		Vector operator * (const T &rhs) const;
		Vector operator / (const T &rhs) const;
		Vector& operator += (const Vector &rhs);
		Vector& operator -= (const Vector &rhs);
		Vector& operator *= (const T &rhs);
		Vector& operator /= (const T &rhs);
		Vector& operator - ();		// FIXME

		T getLength() const;
		T getLengthSquared() const;

		Matrix<T, N, 1>&
		asMatrix();

		const Matrix<T, N, 1>&
		asMatrix() const;

		Matrix<T, 1, N>&
		asTransposedMatrix();

		const Matrix<T, 1, N>&
		asTransposedMatrix() const;

	public:
		static inline uint8_t
		getSize();

		T coords[N];
	};

	template< typename T, uint8_t N >
	struct detail::MakeSigned< Vector<T, N> >
	{ using type = Vector< SignedType<T>, N >; };

	template< typename T, uint8_t N >
	struct detail::MakeUnsigned< Vector<T, N> >
	{ using type = Vector< UnsignedType<T>, N >; };

	template< typename T, uint8_t N >
	struct detail::WideType< Vector<T, N> >
	{ using type = Vector< WideType<T>, N >; };
}

#define IMPLEMENT_VECTOR_ACCESSOR2(a,b)			\
		Vector<T, 2> a##b() const				\
		{										\
			return Vector<T, 2>(a, b);			\
		}

#define IMPLEMENT_VECTOR_ACCESSOR3(a, b, c)		\
		Vector<T, 3> a##b##c() const			\
		{										\
			return Vector<T, 3>(a, b, c);		\
		}

#define IMPLEMENT_VECTOR_ACCESSOR4(a, b, c, d)	\
		Vector<T, 4> a##b##c##d() const			\
		{										\
			return Vector<T, 4>(a, b, c, d);	\
		}

#include "vector_impl.hpp"

#include "vector1.hpp"
#include "vector2.hpp"
#include "vector3.hpp"
#include "vector4.hpp"

#endif // MODM_VECTOR_HPP
