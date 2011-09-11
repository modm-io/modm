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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__LOCATION_2D_HPP
	#error	"Don't include this file directly use 'location.hpp' instead!"
#endif

#include <cmath>
#include <cstdlib>

// -----------------------------------------------------------------------------
template <typename T>
xpcc::Location2D<T>::Location2D() :
	position(), orientation()
{
}

template <typename T>
xpcc::Location2D<T>::Location2D(const Vector<T, 2>& position,
		const float& orientation) :
	position(position),
	orientation(orientation)
{
}

template <typename T>
xpcc::Location2D<T>::Location2D(const T& x, const T& y, const float& orientation) :
	position(x, y),
	orientation(orientation)
{
}

// ----------------------------------------------------------------------------
template <typename T>
const xpcc::Vector<T, 2>&
xpcc::Location2D<T>::getPosition() const
{
	return this->position;
}

template <typename T>
inline const T&
xpcc::Location2D<T>::getX() const
{
	return this->position.x;
}

template <typename T>
inline const T&
xpcc::Location2D<T>::getY() const
{
	return this->position.y;
}

template <typename T>
void
xpcc::Location2D<T>::setPosition(const Vector<T, 2>& point)
{
	this->position = point;
}

template <typename T>
void
xpcc::Location2D<T>::setPosition(const T& x, const T& y)
{
	this->position.set(x, y);
}

template <typename T>
float
xpcc::Location2D<T>::getOrientation() const
{
	return this->orientation;
}

template <typename T>
void
xpcc::Location2D<T>::setOrientation(const float& orientation)
{
	this->orientation = orientation;
}

// -----------------------------------------------------------------------------
template <typename T>
void
xpcc::Location2D<T>::move(const Location2D<T>& diff)
{
	Vector<T, 2> movement = diff.position;
	movement.rotate(this->orientation);
	
	this->position.translate(movement);
	this->orientation = Angle::normalize(this->orientation + diff.orientation);
}

template <typename T>
void
xpcc::Location2D<T>::move(const Vector<T, 2>& diff)
{
	Vector<T, 2> movement(diff);
	movement.rotate(this->orientation);
	
	this->position.translate(movement);
}

template <typename T>
void
xpcc::Location2D<T>::move(T x, float phi)
{
	Vector<T, 2> vector(GeometricTraits<T>::round(x * std::cos(this->orientation)),
					   GeometricTraits<T>::round(x * std::sin(this->orientation)));
	position.translate(vector);
	
	this->orientation = Angle::normalize(this->orientation + phi);
}

// ----------------------------------------------------------------------------
template <typename T>
xpcc::Vector<T, 2>
xpcc::Location2D<T>::translated(const Vector<T, 2>& vector) const
{
	Vector<T, 2> result(vector);
	result.rotate(this->orientation);
	result.translate(this->position);
	
	return result;
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
xpcc::Location2D<U>
xpcc::Location2D<T>::convert() const
{
	return Location2D<U>(this->position.convert<U>(), this->orientation);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
xpcc::Location2D<T>::operator == (const Location2D &other) const
{
	return ((this->position == other.position) &&
			(std::abs(this->orientation - other.orientation) < __FLT_EPSILON__));
}

template<typename T>
bool
xpcc::Location2D<T>::operator != (const Location2D &other) const
{
	return ((this->position != other.position) ||
			(std::abs(this->orientation - other.orientation) > __FLT_EPSILON__));
}

// ----------------------------------------------------------------------------
template<class T>
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& os, const xpcc::Location2D<T>& location)
{
	os << location.position << ", phi=" << location.orientation;
	return os;
}
