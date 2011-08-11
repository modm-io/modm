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

#ifndef XPCC_POINTS__POINT_HPP
#define XPCC_POINTS__POINT_HPP

#include <stdint.h>
#include <xpcc/math/matrix.hpp>

namespace xpcc {
	
	template<typename T, uint8_t W, uint8_t H> class Matrix;
	
	/**
	 * \brief	Class for handling common point operations
	 *
	 * Adapted from the implementation of Gaspard Petit (gaspardpetit@gmail.com).
	 *
	 * \see <a href"http://www-etud.iro.umontreal.ca/~petitg/cpp/point.html">Homepage</a>
	 * 
	 * \ingroup	points
	 * \author Niklas Hauser
	 */
	template<typename T, uint8_t SIZE>
	class Point
	{
	public:
		Point();
		Point(const T *ptData);
		
		Point(const Matrix<T, 1, SIZE> &rhs);
		Point(const Point &rhs);
		
		Point& operator = (const Matrix<T, 1, SIZE> &rhs);
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
		Point operator * (const T &rhs) const;
		Point operator / (const T &rhs) const;
		Point& operator += (const Point &rhs);
		Point& operator -= (const Point &rhs);
		Point& operator *= (const T &rhs);
		Point& operator /= (const T &rhs);
		Point& operator - ();
		
		T length() const;
		T length2() const;
		
		Matrix<T, 1, SIZE>& asMatrix();
		const Matrix<T, 1, SIZE>& asMatrix() const;
		const Matrix<T, SIZE, 1>& asTMatrix() const;
		Matrix<T, SIZE, 1>& asTMatrix();
		
	public:
		static uint8_t size();
		T coords[SIZE];
	};
}

#define IMPLEMENT_POINT_ACCESSOR2(a,b)\
Point<T, 2> a##b() const\
{\
return Point<T, 2>(a, b);\
}

#define IMPLEMENT_POINT_ACCESSOR3(a, b, c)\
Point<T, 3> a##b##c() const\
{\
return Point<T, 3>(a, b, c);\
}


#define IMPLEMENT_POINT_ACCESSOR4(a, b, c, d)\
Point<T, 4> a##b##c##d() const\
{\
return Point<T, 4>(a, b, c, d);\
}

#include "point_impl.hpp"

#endif // XPCC_POINTS__POINT_HPP
