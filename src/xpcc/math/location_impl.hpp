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

#ifndef	XPCC__LOCATION_HPP
	#error	"Don't include this file directly use 'location.hpp' instead!"
#endif

// -----------------------------------------------------------------------------
template <typename T>
xpcc::Location<T>::Location() :
	position(), phi()
{
}

template <typename T>
xpcc::Location<T>::Location(const Position& position, const Angle& phi) :
	position(position),
	phi(phi)
{
}

/*template <typename T>
xpcc::Location<T>::Location(const Position& position, float phi) :
	position(position),
	phi(phi)
{
}*/

template <typename T>
xpcc::Location<T>::Location(const T& x, const T& y, const Angle& phi) :
	position(x, y),
	phi(phi)
{
}

/*template <typename T>
xpcc::Location<T>::Location(const T& x, const T& y, float phi) :
	position(x, y),
	phi(phi)
{
}*/

// -----------------------------------------------------------------------------
template <typename T>
void
xpcc::Location<T>::update(Location<T>& diff)
{
	position += diff.position.rotate(phi);
	phi += diff.phi;
	phi.normalize();
}

template <typename T>
void
xpcc::Location<T>::update(T x, xpcc::Angle& phi)
{
	position += Position(x * cos(this->phi).toFloat(),
						 x * sin(this->phi).toFloat());

	this->phi += phi;
	this->phi.normalize();
}


template <typename T>
const typename xpcc::Location<T>::Position&
xpcc::Location<T>::getPosition() const
{
	return this->position;
}

template <typename T>
void
xpcc::Location<T>::setPosition(const Position& position)
{
	this->position = position;
}

template <typename T>
void
xpcc::Location<T>::setPosition(const T& x, const T& y)
{
	this->position.set(x, y);
}

template <typename T>
const xpcc::Angle&
xpcc::Location<T>::getAngle() const
{
	return this->phi;
}

template <typename T>
void
xpcc::Location<T>::setAngle(const Angle& phi)
{
	this->phi = phi;
}

// ----------------------------------------------------------------------------
template<typename T> template<typename U>
xpcc::Location<T>::operator Location<U>() const
{
	return Location<U>(this->position, this->phi);
}

// ----------------------------------------------------------------------------
template<class T>
xpcc::IOStream&
xpcc::operator<<(xpcc::IOStream& os, const xpcc::Location<T>& l)
{
	os << "position=( " << l.position << " )";
	os << "\nphi     =" << l.phi;

	return os;
}
