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

#ifndef	XPCC__POLAR_COORDINATE_HPP
	#error	"Don't include this file directly use 'math/polar_coordinate.hpp' instead!"
#endif

namespace xpcc
{
	// ------------------------------------------------------------------------
	template<typename T>
	PolarCoordinate<T>::PolarCoordinate(const T r, const Angle& theta) : 
		r(r), theta(theta)
	{
	}

	// ------------------------------------------------------------------------
	template<typename T>
	T
	PolarCoordinate<T>::getLength() const
	{
		return r;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	Angle&
	PolarCoordinate<T>::getAngle() const
	{
		return theta;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	void
	PolarCoordinate<T>::normalize()
	{
		r = 1;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	void
	PolarCoordinate<T>::scale(float a)
	{
		r *= a;
	}

	// ------------------------------------------------------------------------
	template<typename T>
	CartesianCoordinate<T>
	PolarCoordinate<T>::toCartesian()
	{
		return CartesianCoordinate<T>(r * cos(theta).toFloat(),
									  r * sin(theta).toFloat());
	}

	// ------------------------------------------------------------------------
	template<typename T>
	PolarCoordinate<T>
	operator - (const PolarCoordinate<T> &a)
	{
		return PolarCoordinate<T>(a.r, -a.theta);
	}

	// ------------------------------------------------------------------------
	template<typename T>
	bool
	operator == (const PolarCoordinate<T> &a, const PolarCoordinate<T> &b)
	{
		return (a.r == b.r) && (a.theta == b.theta);
	}

	// ------------------------------------------------------------------------
	template<typename T>
	bool
	operator != (const PolarCoordinate<T> &a, const PolarCoordinate<T> &b)
	{
		return (a.r != b.r) || (a.theta != b.theta);
	}
}
