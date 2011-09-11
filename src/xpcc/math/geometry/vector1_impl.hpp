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

#ifndef XPCC__VECTOR1_HPP
	#error	"Don't include this file directly, use 'vector1.hpp' instead!"
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>::Vector() :
	x()
{
}

template<typename T>
xpcc::Vector<T, 1>::Vector(T inX) :
	x(inX)
{
}

template<typename T>
xpcc::Vector<T, 1>::Vector(const xpcc::Matrix<T, 1, 1> &rhs) :
	x(*reinterpret_cast<const T*>(&rhs))
{
}

template<typename T>
xpcc::Vector<T, 1>::Vector(const xpcc::Vector<T, 1> &rhs) :
	x(rhs.x)
{
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector<T, 1>::set(const T& x)
{
	this->x = x;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::Vector<T, 1>::setX(const T& value)
{
	this->x = value;
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Vector<T, 1>::getX() const
{
	return this->x;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>&
xpcc::Vector<T, 1>::operator = (const xpcc::Vector<T, 1> &rhs)
{
	x = rhs.x;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>&
xpcc::Vector<T, 1>::operator = (const xpcc::Matrix<T, 1, 1> &rhs)
{
	x = *reinterpret_cast<const T*>(&rhs);
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::operator == (const xpcc::Vector<T, 1> &rhs) const
{
	return (rhs.x == x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::operator != (const xpcc::Vector<T, 1> &rhs) const
{
	return (rhs.x != x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::operator < (const xpcc::Vector<T, 1> &rhs) const
{
	return (x < rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::operator <= (const xpcc::Vector<T, 1> &rhs) const
{
	return (x <= rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::operator > (const xpcc::Vector<T, 1> &rhs) const
{
	return (x > rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::operator >= (const xpcc::Vector<T, 1> &rhs) const
{
	return (x >= rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
const T&
xpcc::Vector<T, 1>::operator [] (uint8_t index) const
{
	return reinterpret_cast<const T*>(this)[index];
}

template<typename T>
T&
xpcc::Vector<T, 1>::operator [] (uint8_t index)
{
	return reinterpret_cast<T*>(this)[index];
}

// ----------------------------------------------------------------------------
template<typename T>
T*
xpcc::Vector<T, 1>::ptr()
{
	return reinterpret_cast<T*>(this);
}

template<typename T>
const T*
xpcc::Vector<T, 1>::ptr() const
{
	return reinterpret_cast<const T*>(this);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>
xpcc::Vector<T, 1>::operator + (const xpcc::Vector<T, 1> &rhs) const
{
	return xpcc::Vector<T, 1>(x+rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>
xpcc::Vector<T, 1>::operator - (const xpcc::Vector<T, 1> &rhs) const
{
	return xpcc::Vector<T, 1>(x-rhs.x);
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::Vector<T, 1>::operator * (const xpcc::Vector<T, 1> &rhs) const
{
	return x*rhs.x;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>
xpcc::Vector<T, 1>::operator * (const T &rhs) const
{
	return xpcc::Vector<T, 1>(x*rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>
xpcc::Vector<T, 1>::operator / (const T &rhs) const
{
	return xpcc::Vector<T, 1>(x/rhs);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>&
xpcc::Vector<T, 1>::operator += (const xpcc::Vector<T, 1> &rhs)
{
	x += rhs.x;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>&
xpcc::Vector<T, 1>::operator -= (const xpcc::Vector<T, 1> &rhs)
{
	x -= rhs.x;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>&
xpcc::Vector<T, 1>::operator *= (const T &rhs)
{
	x *= rhs;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>&
xpcc::Vector<T, 1>::operator /= (const T &rhs)
{
	x /= rhs;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Vector<T, 1>
xpcc::Vector<T, 1>::operator - () const
{
	return Vector<T, 1>(-x);
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::Vector<T, 1>::getLength() const
{
	return x;
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::Vector<T, 1>::getLengthSquared() const
{
	return x * x;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Matrix<T, 1, 1>&
xpcc::Vector<T, 1>::asMatrix()
{
	return *(xpcc::Matrix<T, 1, 1>*) this;
}

// ----------------------------------------------------------------------------
template<typename T>
const xpcc::Matrix<T, 1, 1>&
xpcc::Vector<T, 1>::asMatrix() const
{
	return *(xpcc::Matrix<T, 1, 1>*) this;
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::hasNan() const
{
	return std::isnan(x);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Vector<T, 1>::hasInf() const
{
	return std::isinf(x);
}

// ----------------------------------------------------------------------------
//template<typename U, typename T>
//static xpcc::Vector<T, 1> operator * (const U &lhs, const xpcc::Vector<T, 1> &rhs)
//{
//	return rhs * lhs;
//}
