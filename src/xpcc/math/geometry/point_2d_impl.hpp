// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2009, Roboterclub Aachen e.V.
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

#ifndef	XPCC__POINT_2D_HPP
	#error	"Don't include this file directly, use 'point_2d.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Point2D<T>::Point2D() :
	x(), y()
{
}

template<typename T>
xpcc::Point2D<T>::Point2D(const T& x, const T& y) : 
	x(x), y(y)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Point2D<T>::setX(const T& value)
{
	this->x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Point2D<T>::setY(const T& value)
{
	this->y = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Point2D<T>::set(const T& x, const T& y)
{
	this->x = x;
	this->y = y;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Point2D<T>::getX() const
{
	return this->x;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Point2D<T>::getY() const
{
	return this->y;
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
xpcc::Point2D<T>::operator Point2D<U>() const
{
	return Point2D<U>(this->x, this->y);
}

// ----------------------------------------------------------------------------
template<typename U>
bool
xpcc::operator == (const Point2D<U> &a, const Point2D<U> &b)
{
	return ((a.x == b.x) && (a.y == b.y));
}

template<typename U>
bool
xpcc::operator != (const Point2D<U> &a, const Point2D<U> &b)
{
	return ((a.x != b.x) || (a.y != b.y));
}

// ----------------------------------------------------------------------------
template <typename U>
xpcc::IOStream&
xpcc::operator <<( xpcc::IOStream& s, const xpcc::Point2D<U>& c)
{
	s << "x=" << c.x << ", y=" << c.y;
	return s;
}
