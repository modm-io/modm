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
	#error	"Don't include this file directly, use 'point.hpp' instead!"
#endif

//#include <string>

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
uint8_t xpcc::Point<T, SIZE>::size()
{
	return SIZE;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>::Point()
{}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>::Point(const T *ptData)
{
	memcpy(coords, ptData, sizeof(T)*SIZE);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>::Point(const xpcc::Matrix<T, 1, SIZE> &rhs)
{
	memcpy(coords, &rhs, sizeof(T)*SIZE);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>::Point(const xpcc::Point<T, SIZE> &rhs)
{
	memcpy(coords, rhs.coords, sizeof(T)*SIZE);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator = (const xpcc::Matrix<T, 1, SIZE> &rhs)
{
	memcpy(coords, &rhs, sizeof(T)*SIZE);
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator = (const xpcc::Point<T, SIZE> &rhs)
{
	memcpy(coords, rhs.coords, sizeof(T)*SIZE);
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
bool xpcc::Point<T, SIZE>::operator == (const xpcc::Point<T, SIZE> &rhs) const
{
	return memcmp(coords, rhs.coords, sizeof(T)*SIZE) == 0;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
bool xpcc::Point<T, SIZE>::operator != (const xpcc::Point<T, SIZE> &rhs) const
{
	return memcmp(coords, rhs.coords, sizeof(T)*SIZE) != 0;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
bool xpcc::Point<T, SIZE>::operator < (const xpcc::Point<T, SIZE> &rhs) const
{
	for (uint8_t i = 0; i < SIZE; ++i)
	{
		if ((*this)[i] < rhs[i])		return true;
		else if ((*this)[i] > rhs[i])	return false;
	}
	return false;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
bool xpcc::Point<T, SIZE>::operator <= (const xpcc::Point<T, SIZE> &rhs) const
{
	for (uint8_t i = 0; i < SIZE; ++i)
	{
		if ((*this)[i] < rhs[i])		return true;
		else if ((*this)[i] > rhs[i])	return false;
	}
	return true;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
bool xpcc::Point<T, SIZE>::operator > (const xpcc::Point<T, SIZE> &rhs) const
{
	for (uint8_t i = 0; i < SIZE; ++i)
	{
		if ((*this)[i] > rhs[i])		return true;
		else if ((*this)[i] < rhs[i])	return false;
	}
	return false;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
bool xpcc::Point<T, SIZE>::operator >= (const xpcc::Point<T, SIZE> &rhs) const
{
	for (uint8_t i = 0; i < SIZE; ++i)
	{
		if ((*this)[i] > rhs[i])		return true;
		else if ((*this)[i] < rhs[i])	return false;
	}
	return true;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
const T& xpcc::Point<T, SIZE>::operator [] (uint8_t index) const
{
	return coords[index];
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
T& xpcc::Point<T, SIZE>::operator [] (uint8_t index)
{
	return coords[index];
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
T* xpcc::Point<T, SIZE>::ptr()
{
	return reinterpret_cast<T*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
const T* xpcc::Point<T, SIZE>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE> xpcc::Point<T, SIZE>::operator + (const xpcc::Point<T, SIZE> &rhs) const
{
	xpcc::Point<T, SIZE> pt;
	for (uint8_t i = 0; i < SIZE; ++i) pt[i] = coords[i] + rhs.coords[i];
	return pt;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE> xpcc::Point<T, SIZE>::operator - (const xpcc::Point<T, SIZE> &rhs) const
{
	xpcc::Point<T, SIZE> pt;
	for (uint8_t i = 0; i < SIZE; ++i) pt[i] = coords[i] - rhs.coords[i];
	return pt;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Matrix<T, 1, SIZE>& xpcc::Point<T, SIZE>::asMatrix()
{
	return *reinterpret_cast<xpcc::Matrix<T, 1, SIZE>*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
const xpcc::Matrix<T, 1, SIZE>& xpcc::Point<T, SIZE>::asMatrix() const
{
	return *reinterpret_cast<const xpcc::Matrix<T, 1, SIZE>*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
const xpcc::Matrix<T, SIZE, 1>& xpcc::Point<T, SIZE>::asTMatrix() const
{
	return *reinterpret_cast<const xpcc::Matrix<T, SIZE, 1>*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Matrix<T, SIZE, 1>& xpcc::Point<T, SIZE>::asTMatrix()
{
	return *reinterpret_cast<xpcc::Matrix<T, SIZE, 1>*>(this);
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
T xpcc::Point<T, SIZE>::operator * (const Point &rhs) const
{
	T v = 0;
	for (uint8_t i = 0; i < SIZE; ++i)
		v += (*this)[i]*rhs[i];

	return v;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE> xpcc::Point<T, SIZE>::operator * (const T &rhs) const
{
	xpcc::Point<T, SIZE> pt;
	for (uint8_t i = 0; i < SIZE; ++i) pt[i] = coords[i] * rhs;
	return pt;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE> xpcc::Point<T, SIZE>::operator / (const T &rhs) const
{
	xpcc::Point<T, SIZE> pt;
	for (uint8_t i = 0; i < SIZE; ++i) pt[i] = coords[i] / rhs;
	return pt;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator += (const Point &rhs)
{
	for (uint8_t i = 0; i < SIZE; ++i) coords[i] += rhs.coords[i];
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator -= (const Point &rhs)
{
	for (uint8_t i = 0; i < SIZE; ++i) coords[i] -= rhs.coords[i];
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator *= (const T &rhs)
{
	for (uint8_t i = 0; i < SIZE; ++i) coords[i] -= rhs;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator /= (const T &rhs)
{
	for (uint8_t i = 0; i < SIZE; ++i) coords[i] /= rhs;
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
xpcc::Point<T, SIZE>& xpcc::Point<T, SIZE>::operator - ()
{
	for (uint8_t i = 0; i < SIZE; ++i) coords[i] = -coords[i];
	return *this;
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
T xpcc::Point<T, SIZE>::length() const
{
	return sqrt(length2());
}

//------------------------------------------------------------------------------
//
template<typename T, uint8_t SIZE>
T xpcc::Point<T, SIZE>::length2() const
{
	T len2 = 0;
	for (uint8_t i = 0; i < SIZE; ++i)
		len2 += (*this)[i]*(*this)[i];
	
	return len2;
}
