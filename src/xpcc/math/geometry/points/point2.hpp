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

#ifndef XPCC_POINTS__POINT2_HPP
#define XPCC_POINTS__POINT2_HPP

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
	 * \ingroup	points
	 * \author Gaspard Petit (gaspardpetit@gmail.com)
	 */
	template<typename T>
	class Point<T, 2>
	{
	public:
		Point();
		Point(T inX, T inY);
		Point(const Point<T, 1> &inX, const Point<T, 1> &inY);
		Point(const T &inX, const Point<T, 1> &inY);
		Point(const Point<T, 1> &inX, const T &inY);
		explicit Point(T inVal);
		Point(const Matrix<T, 1, 2> &rhs);
		Point(const Point &rhs);
		Point& operator = (const Point &rhs);
		Point& operator = (const Matrix<T, 1, 2> &rhs);
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
		T operator ^ (const Point &rhs) const;
		Point operator * (const T &rhs) const;
		Point operator / (const T &rhs) const;
		Point& operator += (const Point &rhs);
		
		Point& operator -= (const Point &rhs);
		Point& operator *= (const T &rhs);
		Point& operator /= (const T &rhs);
		Point& operator - ();
		Point& operator ~ ();
		float length() const;
		float length2() const;
		
		void scale(float newLength);
		Point scaled(float newLength) const;
		
		Point normalized() const;
		void normalize();
		
		Point perpendicular() const;
		
		Matrix<T, 1, 2>& asTMatrix();
		const Matrix<T, 1, 2>& asTMatrix() const;
		Matrix<T, 2, 1>& asMatrix();
		const Matrix<T, 2, 1>& asMatrix() const;
		bool hasNan() const;
		bool hasInf() const;
		
		IMPLEMENT_POINT_ACCESSOR2(x,x); IMPLEMENT_POINT_ACCESSOR2(x,y);
		IMPLEMENT_POINT_ACCESSOR2(y,x); IMPLEMENT_POINT_ACCESSOR2(y,y);
		
		IMPLEMENT_POINT_ACCESSOR3(x,x,x); IMPLEMENT_POINT_ACCESSOR3(x,x,y);
		IMPLEMENT_POINT_ACCESSOR3(x,y,x); IMPLEMENT_POINT_ACCESSOR3(x,y,y);
		IMPLEMENT_POINT_ACCESSOR3(y,x,x); IMPLEMENT_POINT_ACCESSOR3(y,x,y);
		IMPLEMENT_POINT_ACCESSOR3(y,y,x); IMPLEMENT_POINT_ACCESSOR3(y,y,y);
		
		IMPLEMENT_POINT_ACCESSOR4(x,x,x,x); IMPLEMENT_POINT_ACCESSOR4(x,x,x,y);
		IMPLEMENT_POINT_ACCESSOR4(x,x,y,x); IMPLEMENT_POINT_ACCESSOR4(x,x,y,y);
		IMPLEMENT_POINT_ACCESSOR4(x,y,x,x); IMPLEMENT_POINT_ACCESSOR4(x,y,x,y);
		IMPLEMENT_POINT_ACCESSOR4(x,y,y,x); IMPLEMENT_POINT_ACCESSOR4(x,y,y,y);
		
		IMPLEMENT_POINT_ACCESSOR4(y,x,x,x); IMPLEMENT_POINT_ACCESSOR4(y,x,x,y);
		IMPLEMENT_POINT_ACCESSOR4(y,x,y,x); IMPLEMENT_POINT_ACCESSOR4(y,x,y,y);
		IMPLEMENT_POINT_ACCESSOR4(y,y,x,x); IMPLEMENT_POINT_ACCESSOR4(y,y,x,y);
		IMPLEMENT_POINT_ACCESSOR4(y,y,y,x); IMPLEMENT_POINT_ACCESSOR4(y,y,y,y);
		
	public:
		T x;
		T y;
	};
	
	template<typename U, typename T>
	static inline Point<T, 2> operator * (const U &lhs, const Point<T, 2> &rhs);
	
	typedef Point<float, 2> 	Point2f;
	typedef Point<int16_t, 2> 		Point2i;
}

#include "point2_impl.hpp"

#endif // XPCC_POINTS__POINT2_HPP
