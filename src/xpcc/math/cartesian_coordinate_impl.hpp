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
 * $Id: cartesian_coordinate_impl.hpp 94 2009-10-19 18:44:57Z dergraaf $
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__CARTESIAN_COORDINATE_HPP
	#error	"Don't include this file directly use 'math/cartesian_coordinate.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>::CartesianCoordinate(const T& x, const T& y) : 
	x(x), y(y)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::CartesianCoordinate<T>::setX(const T& value)
{
	x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::CartesianCoordinate<T>::setY(const T& value)
{
	y = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::CartesianCoordinate<T>::set(const T& x, const T& y)
{
	this->x = x;
	this->y = y;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::CartesianCoordinate<T>::getX() const
{
	return x;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::CartesianCoordinate<T>::getY() const
{
	return y;
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::CartesianCoordinate<T>::getLength() const
{
	float tx = x;
	float ty = y;
	
	return sqrt(tx*tx + ty*ty);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Angle
xpcc::CartesianCoordinate<T>::getAngle() const
{
	return atan2(y, x);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::normalize(const T& length)
{
	x = x / length;
	y = y / length;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::rotate(const Angle& phi)
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
xpcc::PolarCoordinate<T>
xpcc::CartesianCoordinate<T>::toPolar()
{
	return PolarCoordinate<T>(getLength(), getAngle());
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::operator += (const CartesianCoordinate &other)
{
	this->x += other.x;
	this->y += other.y;
	return *this;
}

// ----------------------------------------------------------------------------	
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::operator -= (const CartesianCoordinate &other)
{
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

// ----------------------------------------------------------------------------

template<typename U>
xpcc::CartesianCoordinate<U>
xpcc::operator - (const CartesianCoordinate<U> &a)
{
	return CartesianCoordinate<U>(-a.x, -a.y);
}

template<typename U>
xpcc::CartesianCoordinate<U>
xpcc::operator - (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b)
{
	return CartesianCoordinate<U>(a.x - b.x, a.y - b.y);
}

template<typename U>
xpcc::CartesianCoordinate<U>
xpcc::operator + (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b)
{
	return CartesianCoordinate<U>(a.x + b.x, a.y + b.y);
}


template<typename U>
bool
xpcc::operator == (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b)
{
	return (a.x == b.x) && (a.y == b.y);
}

template<typename U>
bool
xpcc::operator != (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b)
{
	return (a.x != b.x) || (a.y != b.y);
}

template <typename U>
xpcc::IOStream&
xpcc::operator <<( xpcc::IOStream& s, const xpcc::CartesianCoordinate<U>& c)
{
	s << "x=" << c.x << ", y=" << c.y;
	return s;
}
