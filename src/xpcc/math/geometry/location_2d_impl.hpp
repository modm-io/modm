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

// -----------------------------------------------------------------------------
template <typename T>
xpcc::Location2D<T>::Location2D() :
	point(), phi()
{
}

template <typename T>
xpcc::Location2D<T>::Location2D(const Point& point, const float& phi) :
	point(point),
	phi(phi)
{
}

template <typename T>
xpcc::Location2D<T>::Location2D(const T& x, const T& y, const float& phi) :
	point(x, y),
	phi(phi)
{
}

// -----------------------------------------------------------------------------
template <typename T>
void
xpcc::Location2D<T>::update(Location2D<T>& diff)
{
	this->point += diff.point.rotate(phi);
	this->phi = Angle::normalize(this->phi + diff.phi);
}

template <typename T>
void
xpcc::Location2D<T>::update(T x, float phi)
{
	Vector2D<T> vector(x * std::cos(this->phi),
					   x * std::sin(this->phi));
	point.translate(vector);
	
	this->phi = Angle::normalize(this->phi + phi);
}


template <typename T>
const typename xpcc::Location2D<T>::Point&
xpcc::Location2D<T>::getPoint() const
{
	return this->point;
}

template <typename T>
void
xpcc::Location2D<T>::setPoint(const Point& point)
{
	this->point = point;
}

template <typename T>
void
xpcc::Location2D<T>::setPoint(const T& x, const T& y)
{
	this->point.set(x, y);
}

template <typename T>
float
xpcc::Location2D<T>::getAngle() const
{
	return this->phi;
}

template <typename T>
void
xpcc::Location2D<T>::setAngle(const float& phi)
{
	this->phi = phi;
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
xpcc::Location2D<T>::operator Location2D<U>() const
{
	return Location2D<U>(this->point, this->phi);
}

// ----------------------------------------------------------------------------
template<class T>
xpcc::IOStream&
xpcc::operator << (xpcc::IOStream& os, const xpcc::Location2D<T>& location)
{
	os << location.point << ", phi=" << location.phi;
	return os;
}
