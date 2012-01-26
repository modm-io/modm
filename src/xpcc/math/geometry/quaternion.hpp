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

#ifndef XPCC__QUATERNION_HPP
#define XPCC__QUATERNION_HPP

#include <cmath>

namespace xpcc
{
	// forward declaration
	template<class T, uint8_t N>
	class Vector;
	
	template<class T, uint8_t ROWS, uint8_t COLUMNS>
	class Matrix;
	
	/**
	 * \brief	Quaternion
	 * 
	 * 
	 * 
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/quat.html">Homepage</a>
	 * 
	 * \author	Fabian Greif
	 * \ingroup	geometry
	 */
	template<class T>
	class Quaternion
	{
	public:
		Quaternion();
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
		Quaternion& normalize();
		Quaternion& conjugate();

		Quaternion scaled(float newLength) const;
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

#endif	// XPCC__QUATERNION_HPP
