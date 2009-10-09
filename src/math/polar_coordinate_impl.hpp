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

#ifdef	XPCC__POLAR_COORDINATE_HPP


// ----------------------------------------------------------------------------
template<typename T>
xpcc::PolarCoordinate<T>::PolarCoordinate(T r, Angle& theta) : r(r), theta(theta) {
	// nothing else to do here
}

// ----------------------------------------------------------------------------
template<typename T>
T
xpcc::PolarCoordinate<T>::getLength() const {
	return this->r;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::Angle&
xpcc::PolarCoordinate<T>::getAngle() const {
	return this->theta;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::PolarCoordinate<T>::normalize() {
	this->r = 1;
}

// ----------------------------------------------------------------------------
template<typename T>
void
xpcc::PolarCoordinate<T>::scale(float a) {
	this->r *= a;
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::CartesianCoordinate<T>
xpcc::PolarCoordinate<T>::toCartesian() {
	return CartesianCoordinate<T>(r * cos(theta), r * sin(theta));
}

// ----------------------------------------------------------------------------
template<typename T>
xpcc::PolarCoordinate<T>
operator-(const xpcc::PolarCoordinate<T> &a) {
	return xpcc::PolarCoordinate<T>(a.r, -a.theta);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator==(const xpcc::PolarCoordinate<T> &a, const xpcc::PolarCoordinate<T> &b) {
	return (a.r == b.r) && (a.theta == b.theta);
}

// ----------------------------------------------------------------------------
template<typename T>
bool
operator!=(const xpcc::PolarCoordinate<T> &a, const xpcc::PolarCoordinate<T> &b) {
	return (a.r != b.r) || (a.theta != b.theta);
}


#endif	// XPCC__POLAR_COORDINATE_HPP
