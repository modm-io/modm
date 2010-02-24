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

#ifndef	XPCC__POSITION_HPP
#define	XPCC__POSITION_HPP

#include <math.h>
#include <stdint.h>
#include <xpcc/communication/io/iostream.hpp>

#include "angle.hpp"

namespace xpcc
{
	/**
	 * \brief		Cartesian coordinate
	 *
	 * \ingroup	math
	 * \todo	operators for > < >= <=
	 */
	template<typename T = int16_t>
	class Position
	{
	public:
		Position();
		
		Position(const T& x, const T& y);
		
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
		 * \brief	Normalise length to 1
		 */
		Position&
		normalize();

		/**
		 * \brief	Scale the length by \p factor
		 */
		Position&
		scale(const T& factor);

		Position&
		rotate(const Angle& phi);
		
		Position&
		operator += (const Position &other);
		
		Position&
		operator -= (const Position &other);
		
		template<typename U>
		operator U();

	private:
		T x;
		T y;
		
	private:
		template<typename U>
		friend Position<U>
		operator - (const Position<U> &a);
		
		template<typename U>
		friend Position<U>
		operator - (const Position<U> &a, const Position<U> &b);
		
		template<typename U>
		friend Position<U>
		operator + (const Position<U> &a, const Position<U> &b);
		
		template<typename U>
		friend bool
		operator == (const Position<U> &a, const Position<U> &b);
		
		template<typename U>
		friend bool
		operator != (const Position<U> &a, const Position<U> &b);

		template<typename U>
		friend IOStream&
		operator <<(IOStream& s, const Position<U>& c);
	};
	
	template<typename U>
	Position<U>
	operator - (const Position<U> &a);
	
	template<typename U>
	Position<U>
	operator - (const Position<U> &a, const Position<U> &b);
	
	template<typename U>
	Position<U>
	operator + (const Position<U> &a, const Position<U> &b);
	
	template<typename U>
	bool
	operator == (const Position<U> &a, const Position<U> &b);
	
	template<typename U>
	bool
	operator != (const Position<U> &a, const Position<U> &b);
	
	/**
	 * \brief	Stream operator for \b xpcc::Position<U>
	 * \ingroup	math
	 */
	template<typename U>
	IOStream&
	operator << (IOStream& s, const Position<U>& c);
}

#include "position_impl.hpp"

#endif	// XPCC__POSITION_HPP
