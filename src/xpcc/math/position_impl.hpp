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

#ifndef	XPCC__POSITION_HPP
	#error	"Don't include this file directly use 'math/position.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Position<T>::Position() :
	x(), y()
{
}

template<typename T>
xpcc::Position<T>::Position(const T& x, const T& y) : 
	x(x), y(y)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Position<T>::setX(const T& value)
{
	x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Position<T>::setY(const T& value)
{
	y = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Position<T>::set(const T& x, const T& y)
{
	this->x = x;
	this->y = y;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Position<T>::getX() const
{
	return x;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Position<T>::getY() const
{
	return y;
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::Position<T>::getLength() const
{
	float tx = x;
	float ty = y;
	
	return sqrt(tx*tx + ty*ty);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Angle
xpcc::Position<T>::getAngle() const
{
	return atan2(y, x);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Position<T>&
xpcc::Position<T>::normalize()
{
	T length = this->getLength();
	this->x = this->x / length;
	this->y = this->y / length;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Position<T>&
xpcc::Position<T>::scale(const T& factor)
{
	this->x = this->x * factor;
	this->y = this->y * factor;

	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Position<T>&
xpcc::Position<T>::rotate(const Angle& phi)
{
	float c = cos(phi).toFloat();
	float s = sin(phi).toFloat();
	
	// without rounding the result might be false for T = integer
	T tx =    round(c * this->x - s * this->y);
	this->y = round(s * this->x + c * this->y);
	this->x = tx;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Position<T>&
xpcc::Position<T>::operator += (const Position &other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

// ----------------------------------------------------------------------------	
template<typename T>
xpcc::Position<T>&
xpcc::Position<T>::operator -= (const Position &other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
xpcc::Position<T>::operator Position<U>()
{
	return Position<U>(this->x, this->y);
}

// ----------------------------------------------------------------------------

template<typename U>
xpcc::Position<U>
xpcc::operator - (const Position<U> &a)
{
	return Position<U>(-a.x, -a.y);
}

template<typename U>
xpcc::Position<U>
xpcc::operator - (const Position<U> &a, const Position<U> &b)
{
	return Position<U>(a.x - b.x, a.y - b.y);
}

template<typename U>
xpcc::Position<U>
xpcc::operator + (const Position<U> &a, const Position<U> &b)
{
	return Position<U>(a.x + b.x, a.y + b.y);
}


template<typename U>
bool
xpcc::operator == (const Position<U> &a, const Position<U> &b)
{
	return (a.x == b.x) && (a.y == b.y);
}

template<typename U>
bool
xpcc::operator != (const Position<U> &a, const Position<U> &b)
{
	return (a.x != b.x) || (a.y != b.y);
}

template <typename U>
xpcc::IOStream&
xpcc::operator <<( xpcc::IOStream& s, const xpcc::Position<U>& c)
{
	s << "x=" << c.x << ", y=" << c.y;
	return s;
}
