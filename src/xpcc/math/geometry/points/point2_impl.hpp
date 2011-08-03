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
	#error	"Don't include this file directly, use 'point2.hpp' instead!"
#endif

#include <cmath>

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point()
:
	x(),
	y()
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(T inX, T inY)
:
	x(inX),
	y(inY)
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(const xpcc::Point<T, 1> &inX, const xpcc::Point<T, 1> &inY)
:
	x(inX.x),
	y(inY.x)
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(const T &inX, const xpcc::Point<T, 1> &inY)
:
	x(inX),
	y(inY.x)
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(const xpcc::Point<T, 1> &inX, const T &inY)
:
	x(inX.x),
	y(inY)
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(T inVal)
:
	x(inVal),
	y(inVal)
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(const xpcc::Matrix<T, 1, 2> &rhs)
:
	x(reinterpret_cast<const T*>(&rhs)[0]),
	y(reinterpret_cast<const T*>(&rhs)[1])
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>::Point(const Point &rhs)
:
	x(rhs.x),
	y(rhs.y)
{}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator = (const xpcc::Point<T, 2> &rhs)
{
	x = rhs.x;
	y = rhs.y;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator = (const xpcc::Matrix<T, 1, 2> &rhs)
{
	x = reinterpret_cast<const T*>(&rhs)[0];
	y = reinterpret_cast<const T*>(&rhs)[1];
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::operator == (const xpcc::Point<T, 2> &rhs) const
{
	return (rhs.x == x) && (rhs.y == y);
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::operator != (const xpcc::Point<T, 2> &rhs) const
{
	return (rhs.x != x) || (rhs.y != y);
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::operator < (const xpcc::Point<T, 2> &rhs) const
{
	return (x < rhs.x) || ((x == rhs.x) && (y < rhs.y));
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::operator <= (const xpcc::Point<T, 2> &rhs) const
{
	return (x < rhs.x) || ((x == rhs.x) && (y <= rhs.y));
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::operator > (const xpcc::Point<T, 2> &rhs) const
{
	return (x > rhs.x) || ((x == rhs.x) && (y > rhs.y));
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::operator >= (const xpcc::Point<T, 2> &rhs) const
{
	return (x > rhs.x) || ((x == rhs.x) && (y >= rhs.y));
}

//------------------------------------------------------------------------------
//
template<typename T>
const T& xpcc::Point<T, 2>::operator [] (uint8_t index) const
{
	return reinterpret_cast<const T*>(this)[index];
}

//------------------------------------------------------------------------------
//
template<typename T>
T& xpcc::Point<T, 2>::operator [] (uint8_t index)
{
	return reinterpret_cast<T*>(this)[index];
}

//------------------------------------------------------------------------------
//
template<typename T>
T* xpcc::Point<T, 2>::ptr()
{
	return reinterpret_cast<T*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T>
const T* xpcc::Point<T, 2>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::operator + (const xpcc::Point<T, 2> &rhs) const
{
	return xpcc::Point<T, 2>(x+rhs.x, y+rhs.y);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::operator - (const xpcc::Point<T, 2> &rhs) const
{
	return xpcc::Point<T, 2>(x-rhs.x, y-rhs.y);
}

//------------------------------------------------------------------------------
//
template<typename T>
T xpcc::Point<T, 2>::operator * (const xpcc::Point<T, 2> &rhs) const
{
	return x*rhs.x + y*rhs.y;
}

//------------------------------------------------------------------------------
//
template<typename T>
T xpcc::Point<T, 2>::operator ^ (const xpcc::Point<T, 2> &rhs) const
{
	return x*rhs.y - y*rhs.x;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::operator * (const T &rhs) const
{
	return xpcc::Point<T, 2>(x*rhs, y*rhs);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::operator / (const T &rhs) const
{
	return xpcc::Point<T, 2>(x/rhs, y/rhs);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator += (const xpcc::Point<T, 2> &rhs)
{
	x += rhs.x; y += rhs.y;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator -= (const xpcc::Point<T, 2> &rhs)
{
	x -= rhs.x; y -= rhs.y;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator *= (const T &rhs)
{
	x *= rhs; y *= rhs;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator /= (const T &rhs)
{
	x /= rhs; y /= rhs;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator - ()
{
	x = -x, y = -y;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2>& xpcc::Point<T, 2>::operator ~ ()
{
	*this = perpendicular();
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T>
float xpcc::Point<T, 2>::length() const
{
	return sqrt(length2());
}

//------------------------------------------------------------------------------
//
template<typename T>
float xpcc::Point<T, 2>::length2() const
{
	return x*x+y*y;
}

//------------------------------------------------------------------------------
//
template<typename T>
void xpcc::Point<T, 2>::scale(float newLength)
{
	*this = scaled(newLength);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::scaled(float newLength) const
{
	T scale = newLength/length();
	return *this * scale;
}


//------------------------------------------------------------------------------
//
template<typename T>
void xpcc::Point<T, 2>::normalize()
{
	scale(1.0f);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::normalized() const
{
	return scaled(1.0f);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Point<T, 2> xpcc::Point<T, 2>::perpendicular() const
{
	return xpcc::Point<T, 2>(-y, x);
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Matrix<T, 1, 2>& xpcc::Point<T, 2>::asTMatrix()
{
	return *(xpcc::Matrix<T, 1, 2>*)this;
}

//------------------------------------------------------------------------------
//
template<typename T>
const xpcc::Matrix<T, 1, 2>& xpcc::Point<T, 2>::asTMatrix() const
{
	return *(xpcc::Matrix<T, 1, 2>*)this;
}

//------------------------------------------------------------------------------
//
template<typename T>
xpcc::Matrix<T, 2, 1>& xpcc::Point<T, 2>::asMatrix()
{
	return *(xpcc::Matrix<T, 2, 1>*)this;
}

//------------------------------------------------------------------------------
//
template<typename T>
const xpcc::Matrix<T, 2, 1>& xpcc::Point<T, 2>::asMatrix() const
{
	return *(xpcc::Matrix<T, 2, 1>*)this;
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::hasNan() const
{
	return isnan(x) || isnan(y);
}

//------------------------------------------------------------------------------
//
template<typename T>
bool xpcc::Point<T, 2>::hasInf() const
{
	return isinf(x) || isinf(y);
}

//------------------------------------------------------------------------------
//
template<class U, class T>
static inline xpcc::Point<T, 2> operator * (const U &lhs, const xpcc::Point<T, 2> &rhs)
{
	return rhs * lhs;
}
