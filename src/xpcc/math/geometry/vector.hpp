// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__VECTOR_HPP
#define XPCC__VECTOR_HPP

#include <cmath>
#include <stdint.h>

#include <xpcc/math/matrix.hpp>

namespace xpcc
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
	 * \ingroup	geometry
	 * \author	Niklas Hauser
	 */
	template<typename T, uint8_t N>
	class Vector
	{
	public:
		Vector();
		Vector(const T *ptData);
		
		Vector(const Matrix<T, N, 1> &rhs);
		Vector(const Vector &rhs);
		
		Vector& operator = (const Matrix<T, N, 1> &rhs);
		Vector& operator = (const Vector &rhs);
		
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

	// ------------------------------------------------------------------------
	// Declaration of specialized methods
	// ------------------------------------------------------------------------
	template<uint8_t N>
	float
	Vector<float, N>::getLength() const
	{
		return sqrtf(getLengthSquared());
	}
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

#endif // XPCC__VECTOR_HPP
