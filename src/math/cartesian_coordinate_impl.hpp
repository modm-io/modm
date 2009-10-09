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

#ifdef	XPCC__CARTESIAN_COORDINATE_HPP

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>::CartesianCoordinate(T x, T y) : x(x), y(y) {
	// nothing else to do here
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::CartesianCoordinate<T>::length() const {
	float tx = x;
	float ty = y;
	
	return sqrt(tx*tx + ty*ty);
}

#if defined(__AVR__) && defined(__AVR_HAVE_MUL__)
#include "../hal/avr/math.hpp"
// ----------------------------------------------------------------------------
template<>
int16_t
xpcc::CartesianCoordinate<int16_t>::length() const {
	int32_t t;
	t = avr::mul32(x, x);
	t = avr::mac32(t, y, y);
	
	return avr::sqrt32_round(t);
}
#endif

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Angle
xpcc::CartesianCoordinate<T>::angle() const {
	return Angle(atan2(this->y, this->x));
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::normalize() {
	T length(this->length());
	
	this->x = this->x / length;
	this->y = this->y / length;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::rotate(Angle& phi) {
	float c = cos(phi);
	float s = sin(phi);
	
	T tx = 		c * this->x - s * this->y;
	this->y =	s * this->x + c * this->y;
	this->x = tx;
	
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::PolarCoordinate<T>
xpcc::CartesianCoordinate<T>::toPolar() {
	return PolarCoordinate<T>(this->length(), this->angle());
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::operator+=(const CartesianCoordinate &other) {
	this->x += other.x;
	this->y += other.y;
	return *this;
}

// ----------------------------------------------------------------------------	
template<typename T>
xpcc::CartesianCoordinate<T>&
xpcc::CartesianCoordinate<T>::operator-=(const CartesianCoordinate &other) {
	this->x -= other.x;
	this->y -= other.y;
	return *this;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>
operator-(const xpcc::CartesianCoordinate<T> &a) {
	return xpcc::CartesianCoordinate<T>(-a.x, -a.y);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>
operator-(const xpcc::CartesianCoordinate<T> &a, const xpcc::CartesianCoordinate<T> &b) {
	return xpcc::CartesianCoordinate<T>(a.x - b.x, a.y - b.y);
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>
operator+(const xpcc::CartesianCoordinate<T> &a, const xpcc::CartesianCoordinate<T> &b) {
	return xpcc::CartesianCoordinate<T>(a.x + b.x, a.y + b.y);
}


// ----------------------------------------------------------------------------
template<typename T>
bool
operator==(const xpcc::CartesianCoordinate<T> &a, const xpcc::CartesianCoordinate<T> &b) {
	return (a.x == b.x) && (a.y == b.y);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator!=(const xpcc::CartesianCoordinate<T> &a, const xpcc::CartesianCoordinate<T> &b) {
	return (a.x != b.x) || (a.y != b.y);
}

#endif	// XPCC__CARTESIAN_COORDINATE_HPP
