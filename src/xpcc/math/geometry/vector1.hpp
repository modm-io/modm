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
 */
// ----------------------------------------------------------------------------

#ifndef XPCC__VECTOR1_HPP
#define XPCC__VECTOR1_HPP

#include <cmath>
#include <cstdlib>
#include <stdint.h>

#include "vector.hpp"

namespace xpcc
{
	/**
	 * \brief	Class for handling common vector operations (1D)
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 * 
	 * \ingroup	geometry
	 * \author	Niklas Hauser
	 */
	template<typename T>
	class Vector<T, 1>
	{
	public:
		Vector();
		Vector(T inX);
		Vector(const Matrix<T, 1, 1> &rhs);
		Vector(const Vector &rhs);
		
		inline void
		set(const T& x);
		
		inline void
		setX(const T& value);
		
		inline const T&
		getX() const;
		
		
		Vector& operator = (const Vector &rhs);
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

#endif // XPCC__VECTOR1_HPP
