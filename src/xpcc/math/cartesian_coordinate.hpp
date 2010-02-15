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

#ifndef	XPCC__CARTESIAN_COORDINATE_HPP
#define	XPCC__CARTESIAN_COORDINATE_HPP

#include <math.h>
#include <stdint.h>
#include <xpcc/communication/io/iostream.hpp>

#include "angle.hpp"
#include "polar_coordinate.hpp"

namespace xpcc
{
	// forward declaration, needed because of circular reference 
	// with class PolarCoordinate
	template<typename T>
	class PolarCoordinate;

	/**
	 * \brief		Cartesian coordinate
	 *
	 * \ingroup	math
	 * \todo	operators for > < >= <=
	 */
	template<typename T>
	class CartesianCoordinate
	{
	public:
		CartesianCoordinate(const T& x = T(), const T& y = T());
		
		void
		setX(const T& value);
		
		void
		setY(const T& value);
		
		void
		set(const T& x, const T& y);
		
		const T&
		getX() const;
		
		const T&
		getY() const;
		
		/**
		 * \brief	Calculate length
		 */
		T
		getLength() const;
		
		/**
		 * \brief	Calculate the absolute angle
		 */
		Angle
		getAngle() const;
		
		/**
		 * \brief	Normalize length to 1
		 */
		CartesianCoordinate&
		normalize(const T& length);
		
		CartesianCoordinate&
		rotate(const Angle& phi);
		
		/**
		 * \brief	Transform to polar coordiante system
		 */
		PolarCoordinate<T>
		toPolar();
		
		CartesianCoordinate&
		operator += (const CartesianCoordinate &other);
		
		CartesianCoordinate&
		operator -= (const CartesianCoordinate &other);
		
	private:
		T x;
		T y;
		
	private:
		template<typename U>
		friend CartesianCoordinate<U>
		operator - (const CartesianCoordinate<U> &a);
		
		template<typename U>
		friend CartesianCoordinate<U>
		operator - (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
		
		template<typename U>
		friend CartesianCoordinate<U>
		operator + (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
		
		template<typename U>
		friend bool
		operator == (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
		
		template<typename U>
		friend bool
		operator != (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);

		template<typename U>
		friend IOStream&
		operator <<(IOStream& s, const CartesianCoordinate<U>& c);
	};
	
	template<typename U>
	CartesianCoordinate<U>
	operator - (const CartesianCoordinate<U> &a);
	
	template<typename U>
	CartesianCoordinate<U>
	operator - (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
	
	template<typename U>
	CartesianCoordinate<U>
	operator + (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
	
	template<typename U>
	bool
	operator == (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
	
	template<typename U>
	bool
	operator != (const CartesianCoordinate<U> &a, const CartesianCoordinate<U> &b);
	
	/**
	 * \brief	Stream operator for \b xpcc::CartesianCoordinate<U>
	 * \ingroup	math
	 */
	template<typename U>
	IOStream&
	operator << (IOStream& s, const CartesianCoordinate<U>& c);
}

#include "cartesian_coordinate_impl.hpp"

#endif	// XPCC__CARTESIAN_COORDINATE_HPP
