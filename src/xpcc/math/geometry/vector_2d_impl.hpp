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

#ifndef	XPCC__VECTOR_2D_HPP
	#error	"Don't include this file directly, use 'point.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>::Vector2D() :
	x(), y()
{
}

template<typename T>
xpcc::Vector2D<T>::Vector2D(const T& x, const T& y) : 
	x(x), y(y)
{
}

template<typename T>
xpcc::Vector2D<T>::Vector2D(const Vector2D<T>& A, const Vector2D<T>& B) :
	x(B.x - A.x), y(B.y - A.y)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector2D<T>::setX(const T& value)
{
	this->x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector2D<T>::setY(const T& value)
{
	this->y = value;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector2D<T>::set(const T& x, const T& y)
{
	this->x = x;
	this->y = y;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Vector2D<T>::getX() const
{
	return this->x;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Vector2D<T>::getY() const
{
	return this->y;
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::Vector2D<T>::getLength() const
{
	float tx = this->x;
	float ty = this->y;
	
	return GeometricTraits<T>::round(std::sqrt(tx*tx + ty*ty));
}

// ----------------------------------------------------------------------------
template<typename T>
typename xpcc::Vector2D<T>::WideType
xpcc::Vector2D<T>::getLengthSquared() const
{
	WideType tx = static_cast<WideType>(this->x);
	WideType ty = static_cast<WideType>(this->y);
	
	return tx * tx + ty * ty;
}

// ----------------------------------------------------------------------------
template<typename T>
float
xpcc::Vector2D<T>::getAngle() const
{
	return std::atan2(this->y, this->x);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>&
xpcc::Vector2D<T>::normalize()
{
	T length = this->getLength();
	
	this->x = this->x / length;
	this->y = this->y / length;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
template<typename U>
xpcc::Vector2D<T>&
xpcc::Vector2D<T>::scale(const U& factor)
{
	this->x = this->x * factor;
	this->y = this->y * factor;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>&
xpcc::Vector2D<T>::rotate(float phi)
{
	float c = std::cos(phi);
	float s = std::sin(phi);
	
	// without rounding the result might be false for T = integer
	T tx =    GeometricTraits<T>::round(c * this->x - s * this->y);
	this->y = GeometricTraits<T>::round(s * this->x + c * this->y);
	this->x = tx;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>&
xpcc::Vector2D<T>::translate(const Vector2D<T>& vector)
{
	this->x += vector.x;
	this->y += vector.y;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
typename xpcc::Vector2D<T>::WideType
xpcc::Vector2D<T>::getDistanceTo(const Vector2D<T>& other) const
{
	return Vector2D<T>(*this, other).getLength();
}

// ----------------------------------------------------------------------------
template<typename T>
int_fast8_t
xpcc::Vector2D<T>::ccw(const Vector2D<T>& a, const Vector2D<T>& b,
		const Vector2D<T>& c)
{
	WideType dx1 = b.x - a.x;
	WideType dy1 = b.y - a.y;
	WideType dx2 = c.x - a.x;
	WideType dy2 = c.y - a.y;
	
	WideType d1 = dx1 * dy2;
	WideType d2 = dy1 * dx2;
	
	if (d1 > d2) {
		return 1;
	}
	else if (d1 < d2) {
		return -1;
	}
	else
	{
		if ((dx1 * dx2 < 0) || (dy1 * dy2 < 0)) {
			return -1;
		}
		else
		{
			if ((dx1 * dx1 + dy1 * dy1) >= (dx2 * dx2 + dy2 * dy2)) {
				return 0;
			}
			else {
				return 1;
			}
		}
	}
}

// ----------------------------------------------------------------------------
template<typename T>
typename xpcc::Vector2D<T>::WideType
xpcc::Vector2D<T>::dot(const xpcc::Vector2D<T>& other) const
{
	return (static_cast<WideType>(this->x) * static_cast<WideType>(other.x) +
			static_cast<WideType>(this->y) * static_cast<WideType>(other.y));
}

// ----------------------------------------------------------------------------
template<typename T>
typename xpcc::Vector2D<T>::WideType
xpcc::Vector2D<T>::cross(const xpcc::Vector2D<T>& other) const
{
	return (static_cast<WideType>(this->x) * static_cast<WideType>(other.y) -
			static_cast<WideType>(this->y) * static_cast<WideType>(other.x));
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
xpcc::Vector2D<U>
xpcc::Vector2D<T>::convert() const
{
	return Vector2D<U>(static_cast<U>(this->x), static_cast<U>(this->y));
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>
xpcc::Vector2D<T>::toOrthogonalVector() const
{
	return Vector2D<T>(this->y, -this->x);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>
xpcc::Vector2D<T>::operator - () const
{
	return Vector2D(-this->x, -this->y);
}

template<typename T>
xpcc::Vector2D<T>
xpcc::Vector2D<T>::operator - (const Vector2D &other) const
{
	return Vector2D<T>(this->x - other.x, this->y - other.y);
}

template<typename T>
xpcc::Vector2D<T>
xpcc::Vector2D<T>::operator + (const Vector2D &other) const
{
	return Vector2D<T>(this->x + other.x, this->y + other.y);
}

template<typename T>
xpcc::Vector2D<T>&
xpcc::Vector2D<T>::operator += (const Vector2D &other)
{
	this->x += other.x;
	this->y += other.y;
	
	return *this;
}

template<typename T>
xpcc::Vector2D<T>&
xpcc::Vector2D<T>::operator -= (const Vector2D &other)
{
	this->x -= other.x;
	this->y -= other.y;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector2D<T>
xpcc::Vector2D<T>::operator * (float scale) const
{
	return Vector2D<T>(GeometricTraits<T>::round(this->x * scale),
					   GeometricTraits<T>::round(this->y * scale));
}

template<typename T>
xpcc::Vector2D<T>
xpcc::Vector2D<T>::operator / (float scale) const
{
	return Vector2D<T>(GeometricTraits<T>::round(this->x / scale),
					   GeometricTraits<T>::round(this->y / scale));
}

// ----------------------------------------------------------------------------	
template<typename T>
bool
xpcc::Vector2D<T>::operator == (const Vector2D &other) const
{
	return ((this->x == other.x) && (this->y == other.y));
}

template<typename T>
bool
xpcc::Vector2D<T>::operator != (const Vector2D &other) const
{
	return ((this->x != other.x) || (this->y != other.y));
}

// ----------------------------------------------------------------------------
// Global functions
// ----------------------------------------------------------------------------
template <typename U>
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& s, const xpcc::Vector2D<U>& c)
{
	s << "x=" << c.x << ", y=" << c.y;
	return s;
}

// ----------------------------------------------------------------------------
template<typename U>
xpcc::Vector2D<U>
xpcc::operator * (float scale, const Vector2D<U> &vector)
{
	// invoke member function
	return vector * scale;
}

template<typename U>
xpcc::Vector2D<U>
xpcc::operator / (float scale, const Vector2D<U> &vector)
{
	// invoke member function
	return vector / scale;
}
