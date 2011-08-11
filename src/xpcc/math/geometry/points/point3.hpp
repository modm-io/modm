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

#ifndef XPCC_POINTS__POINT3_HPP
#define XPCC_POINTS__POINT3_HPP

#include <stdint.h>
#include "point.hpp"

namespace xpcc {
	/**
	 * \brief	Class for handling common point operations
	 *	+ : addition of points
	 *	- : different of points
	 *	* : dot product or scalar multiplication
	 *	/ : scalar division
	 *	^ : cross product (determinant)
	 *	~ : perpendicular
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 * 
	 * \ingroup	points
	 * \author Niklas Hauser
	 */
	template<typename T>
	class Point<T, 3>
	{
	public:
		Point();
		
		template<typename U>
		explicit Point(const U *array);
		
		explicit Point(T inVal);
		Point(T inX, T inY, T inZ);
		
		Point(const T &inX, const T &inY, const Point<T, 1> &inZ);
		Point(const Point<T, 1> &inX, const T &inY, const T &inZ);
		
		Point(const Point<T, 1> &inX, const T &inY, const Point<T, 1> &inZ);
		Point(const Point<T, 1> &inX, const Point<T, 1> &inY, const T &inZ);
		Point(const T &inX, const Point<T, 1> &inY, const Point<T, 1> &inZ);
		
		Point(const Point<T, 1> &inX, const Point<T, 1> &inY, const Point<T, 1> &inZ);
		
		Point(const Point<T,2> &inXY, const T &inZ);
		Point(const T &inX, const Point<T, 2> &inYZ);
		
		Point(const Point<T,2> &inXY, const Point<T, 1> &inZ);
		Point(const Point<T, 1> &inX, const Point<T, 2> &inYZ);
		
		template<typename U>
		Point(const Point<U, 3> &rhs);
		
		Point(const Matrix<T, 1, 3> &rhs);
		
		
		Point& operator = (const Matrix<T, 1, 3> &rhs);
		Point& operator = (const Point &rhs);
		
		bool operator == (const Point &rhs) const;
		bool operator != (const Point &rhs) const;
		bool operator < (const Point &rhs) const;
		bool operator <= (const Point &rhs) const;
		bool operator > (const Point &rhs) const;
		bool operator >= (const Point &rhs) const;
		const T& operator [] (uint8_t index) const;
		T& operator [] (uint8_t index);
		T* ptr();
		const T* ptr() const;
		Point operator + (const Point &rhs) const;
		Point operator - (const Point &rhs) const;
		
		T operator * (const Point &rhs) const;
		Point operator ^ (const Point &rhs) const;
		Point operator * (const T &rhs) const;
		Point operator / (const T &rhs) const;
		Point& operator += (const Point &rhs);
		Point& operator -= (const Point &rhs);
		Point& operator *= (const T &rhs);
		Point& operator /= (const T &rhs);
		Point& operator - ();
		float length() const;
		float length2() const;
		
		Point scaled(float newLength) const;
		void scale(float newLength);
		
		Point normalized() const;
		void normalize();
		
		Matrix<T, 1, 3>& asTMatrix();
		const Matrix<T, 1, 3>& asTMatrix() const;
		Matrix<T, 3, 1>& asMatrix();
		const Matrix<T, 3, 1>& asMatrix() const;
		bool hasNan() const;
		bool hasInf() const;
		
		IMPLEMENT_POINT_ACCESSOR2(x,x); IMPLEMENT_POINT_ACCESSOR2(x,y); IMPLEMENT_POINT_ACCESSOR2(x,z);
		IMPLEMENT_POINT_ACCESSOR2(y,x); IMPLEMENT_POINT_ACCESSOR2(y,y); IMPLEMENT_POINT_ACCESSOR2(y,z);
		IMPLEMENT_POINT_ACCESSOR2(z,x); IMPLEMENT_POINT_ACCESSOR2(z,y); IMPLEMENT_POINT_ACCESSOR2(z,z);
		
		IMPLEMENT_POINT_ACCESSOR3(x,x,x); IMPLEMENT_POINT_ACCESSOR3(x,x,y); IMPLEMENT_POINT_ACCESSOR3(x,x,z);
		IMPLEMENT_POINT_ACCESSOR3(x,y,x); IMPLEMENT_POINT_ACCESSOR3(x,y,y); IMPLEMENT_POINT_ACCESSOR3(x,y,z);
		IMPLEMENT_POINT_ACCESSOR3(x,z,x); IMPLEMENT_POINT_ACCESSOR3(x,z,y); IMPLEMENT_POINT_ACCESSOR3(x,z,z);
		IMPLEMENT_POINT_ACCESSOR3(y,x,x); IMPLEMENT_POINT_ACCESSOR3(y,x,y); IMPLEMENT_POINT_ACCESSOR3(y,x,z);
		IMPLEMENT_POINT_ACCESSOR3(y,y,x); IMPLEMENT_POINT_ACCESSOR3(y,y,y); IMPLEMENT_POINT_ACCESSOR3(y,y,z);
		IMPLEMENT_POINT_ACCESSOR3(y,z,x); IMPLEMENT_POINT_ACCESSOR3(y,z,y); IMPLEMENT_POINT_ACCESSOR3(y,z,z);
		IMPLEMENT_POINT_ACCESSOR3(z,x,x); IMPLEMENT_POINT_ACCESSOR3(z,x,y); IMPLEMENT_POINT_ACCESSOR3(z,x,z);
		IMPLEMENT_POINT_ACCESSOR3(z,y,x); IMPLEMENT_POINT_ACCESSOR3(z,y,y); IMPLEMENT_POINT_ACCESSOR3(z,y,z);
		IMPLEMENT_POINT_ACCESSOR3(z,z,x); IMPLEMENT_POINT_ACCESSOR3(z,z,y); IMPLEMENT_POINT_ACCESSOR3(z,z,z);
		
		IMPLEMENT_POINT_ACCESSOR4(x,x,x,x); IMPLEMENT_POINT_ACCESSOR4(x,x,x,y); IMPLEMENT_POINT_ACCESSOR4(x,x,x,z);
		IMPLEMENT_POINT_ACCESSOR4(x,x,y,x); IMPLEMENT_POINT_ACCESSOR4(x,x,y,y); IMPLEMENT_POINT_ACCESSOR4(x,x,y,z);
		IMPLEMENT_POINT_ACCESSOR4(x,x,z,x); IMPLEMENT_POINT_ACCESSOR4(x,x,z,y); IMPLEMENT_POINT_ACCESSOR4(x,x,z,z);
		IMPLEMENT_POINT_ACCESSOR4(x,y,x,x); IMPLEMENT_POINT_ACCESSOR4(x,y,x,y); IMPLEMENT_POINT_ACCESSOR4(x,y,x,z);
		IMPLEMENT_POINT_ACCESSOR4(x,y,y,x); IMPLEMENT_POINT_ACCESSOR4(x,y,y,y); IMPLEMENT_POINT_ACCESSOR4(x,y,y,z);
		IMPLEMENT_POINT_ACCESSOR4(x,y,z,x); IMPLEMENT_POINT_ACCESSOR4(x,y,z,y); IMPLEMENT_POINT_ACCESSOR4(x,y,z,z);
		IMPLEMENT_POINT_ACCESSOR4(x,z,x,x); IMPLEMENT_POINT_ACCESSOR4(x,z,x,y); IMPLEMENT_POINT_ACCESSOR4(x,z,x,z);
		IMPLEMENT_POINT_ACCESSOR4(x,z,y,x); IMPLEMENT_POINT_ACCESSOR4(x,z,y,y); IMPLEMENT_POINT_ACCESSOR4(x,z,y,z);
		IMPLEMENT_POINT_ACCESSOR4(x,z,z,x); IMPLEMENT_POINT_ACCESSOR4(x,z,z,y); IMPLEMENT_POINT_ACCESSOR4(x,z,z,z);
		
		IMPLEMENT_POINT_ACCESSOR4(y,x,x,x); IMPLEMENT_POINT_ACCESSOR4(y,x,x,y); IMPLEMENT_POINT_ACCESSOR4(y,x,x,z);
		IMPLEMENT_POINT_ACCESSOR4(y,x,y,x); IMPLEMENT_POINT_ACCESSOR4(y,x,y,y); IMPLEMENT_POINT_ACCESSOR4(y,x,y,z);
		IMPLEMENT_POINT_ACCESSOR4(y,x,z,x); IMPLEMENT_POINT_ACCESSOR4(y,x,z,y); IMPLEMENT_POINT_ACCESSOR4(y,x,z,z);
		IMPLEMENT_POINT_ACCESSOR4(y,y,x,x); IMPLEMENT_POINT_ACCESSOR4(y,y,x,y); IMPLEMENT_POINT_ACCESSOR4(y,y,x,z);
		IMPLEMENT_POINT_ACCESSOR4(y,y,y,x); IMPLEMENT_POINT_ACCESSOR4(y,y,y,y); IMPLEMENT_POINT_ACCESSOR4(y,y,y,z);
		IMPLEMENT_POINT_ACCESSOR4(y,y,z,x); IMPLEMENT_POINT_ACCESSOR4(y,y,z,y); IMPLEMENT_POINT_ACCESSOR4(y,y,z,z);
		IMPLEMENT_POINT_ACCESSOR4(y,z,x,x); IMPLEMENT_POINT_ACCESSOR4(y,z,x,y); IMPLEMENT_POINT_ACCESSOR4(y,z,x,z);
		IMPLEMENT_POINT_ACCESSOR4(y,z,y,x); IMPLEMENT_POINT_ACCESSOR4(y,z,y,y); IMPLEMENT_POINT_ACCESSOR4(y,z,y,z);
		IMPLEMENT_POINT_ACCESSOR4(y,z,z,x); IMPLEMENT_POINT_ACCESSOR4(y,z,z,y); IMPLEMENT_POINT_ACCESSOR4(y,z,z,z);
		
		IMPLEMENT_POINT_ACCESSOR4(z,x,x,x); IMPLEMENT_POINT_ACCESSOR4(z,x,x,y); IMPLEMENT_POINT_ACCESSOR4(z,x,x,z);
		IMPLEMENT_POINT_ACCESSOR4(z,x,y,x); IMPLEMENT_POINT_ACCESSOR4(z,x,y,y); IMPLEMENT_POINT_ACCESSOR4(z,x,y,z);
		IMPLEMENT_POINT_ACCESSOR4(z,x,z,x); IMPLEMENT_POINT_ACCESSOR4(z,x,z,y); IMPLEMENT_POINT_ACCESSOR4(z,x,z,z);
		IMPLEMENT_POINT_ACCESSOR4(z,y,x,x); IMPLEMENT_POINT_ACCESSOR4(z,y,x,y); IMPLEMENT_POINT_ACCESSOR4(z,y,x,z);
		IMPLEMENT_POINT_ACCESSOR4(z,y,y,x); IMPLEMENT_POINT_ACCESSOR4(z,y,y,y); IMPLEMENT_POINT_ACCESSOR4(z,y,y,z);
		IMPLEMENT_POINT_ACCESSOR4(z,y,z,x); IMPLEMENT_POINT_ACCESSOR4(z,y,z,y); IMPLEMENT_POINT_ACCESSOR4(z,y,z,z);
		IMPLEMENT_POINT_ACCESSOR4(z,z,x,x); IMPLEMENT_POINT_ACCESSOR4(z,z,x,y); IMPLEMENT_POINT_ACCESSOR4(z,z,x,z);
		IMPLEMENT_POINT_ACCESSOR4(z,z,y,x); IMPLEMENT_POINT_ACCESSOR4(z,z,y,y); IMPLEMENT_POINT_ACCESSOR4(z,z,y,z);
		IMPLEMENT_POINT_ACCESSOR4(z,z,z,x); IMPLEMENT_POINT_ACCESSOR4(z,z,z,y); IMPLEMENT_POINT_ACCESSOR4(z,z,z,z);
		
	public:
		T x;
		T y;
		T z;
	};
	
	template<typename T>
	static inline Point<T, 3> operator * (const T &lhs, const Point<T, 3> &rhs);
	
	template<typename T>
	static inline Point<T, 3> operator * (const Matrix<T, 3, 3> &lhs, const Point<T, 3> &rhs);
	
	
	typedef Point<float, 3> 	Point3f;
	typedef Point<int16_t, 3>		Point3i;
}

#include "point3_impl.hpp"

#endif // XPCC_POINTS__POINT3_HPP
