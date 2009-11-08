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

#if defined(__AVR__) && defined(__AVR_HAVE_MUL__)
#include <xpcc/hal/peripheral/avr/math/math.hpp>
#endif

namespace xpcc
{
	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>::CartesianCoordinate(const T& x, const T& y) : x(x), y(y)
	{
	}
	
	// ------------------------------------------------------------------------
	template<typename T>
	void
	CartesianCoordinate<T>::setX(const T& value)
	{
		x = value;
	}
	
	// ------------------------------------------------------------------------
	template<typename T>
	void
	CartesianCoordinate<T>::setY(const T& value)
	{
		y = value;
	}
	
	// ------------------------------------------------------------------------
	template<typename T>
	void
	CartesianCoordinate<T>::set(const T& x, const T& y)
	{
		this->x = x;
		this->y = y;
	}
	
	// ------------------------------------------------------------------------
	template<typename T>
	const T&
	CartesianCoordinate<T>::getX() const
	{
		return x;
	}
	
	// ------------------------------------------------------------------------
	template<typename T>
	const T&
	CartesianCoordinate<T>::getY() const
	{
		return y;
	}
	
	// ------------------------------------------------------------------------
	template<typename T>
	T
	CartesianCoordinate<T>::getLength() const
	{
		float tx = x;
		float ty = y;
		
		return sqrt(tx*tx + ty*ty);
	}

#if defined(__AVR__) && defined(__AVR_HAVE_MUL__)
	template<>
	int16_t
	CartesianCoordinate<int16_t>::getLength() const
	{
		int32_t t;
		t = avr::mul32(x, x);
		t = avr::mac32(t, y, y);
		
		return avr::sqrt32_round(t);
	}

#endif

	// ------------------------------------------------------------------------
	template<typename T>
	Angle
	CartesianCoordinate<T>::getAngle() const
	{
		return atan2(y, x);
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>&
	CartesianCoordinate<T>::normalize(const T& length)
	{
		x = x / length;
		y = y / length;
		
		return *this;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>&
	CartesianCoordinate<T>::rotate(const Angle& phi)
	{
		float c = cos(phi).toFloat();
		float s = sin(phi).toFloat();
		
		// without rounding the result might be false for T = integer
		T tx =    round(c * this->x - s * this->y);
		this->y = round(s * this->x + c * this->y);
		this->x = tx;
		
		return *this;
	}
	
	template<>
	CartesianCoordinate<float>&
	CartesianCoordinate<float>::rotate(const Angle& phi)
	{
		float c = cos(phi).toFloat();
		float s = sin(phi).toFloat();
		
		float tx = (c * this->x - s * this->y);
		this->y =  (s * this->x + c * this->y);
		this->x = tx;
		
		return *this;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	PolarCoordinate<T>
	CartesianCoordinate<T>::toPolar()
	{
		return PolarCoordinate<T>(getLength(), getAngle());
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>&
	CartesianCoordinate<T>::operator += (const CartesianCoordinate &other)
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	// ------------------------------------------------------------------------	
	template<typename T>
	CartesianCoordinate<T>&
	CartesianCoordinate<T>::operator -= (const CartesianCoordinate &other)
	{
		this->x -= other.x;
		this->y -= other.y;
		return *this;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>
	operator - (const CartesianCoordinate<T> &a)
	{
		return CartesianCoordinate<T>(-a.x, -a.y);
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>
	operator - (const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b)
	{
		return CartesianCoordinate<T>(a.x - b.x, a.y - b.y);
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>
	operator + (const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b)
	{
		return CartesianCoordinate<T>(a.x + b.x, a.y + b.y);
	}


	// ------------------------------------------------------------------------
	template<typename T>
	bool
	operator == (const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b)
	{
		return (a.x == b.x) && (a.y == b.y);
	}

	// ------------------------------------------------------------------------
	template<typename T>
	bool
	operator != (const CartesianCoordinate<T> &a, const CartesianCoordinate<T> &b)
	{
		return (a.x != b.x) || (a.y != b.y);
	}
}
