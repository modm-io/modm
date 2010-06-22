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

#ifndef	XPCC__POINT_2D_HPP
#define	XPCC__POINT_2D_HPP

#include <cmath>
#include <stdint.h>

#include <xpcc/io/iostream.hpp>

namespace xpcc
{
	/**
	 * \brief	2D Point
	 * 
	 * 
	 * 
	 * A point is specified by a x coordinate and an y coordinate which can
	 * be accessed using the getX() and getY() functions.
	 * 
	 * \ingroup	math
	 */
	template<typename T = int16_t>
	class Point2D
	{
	public:
		/**
		 * \brief	Default-Constructor
		 * 
		 * Creates a Point with coordinates (0, 0).
		 */
		Point2D();
		
		Point2D(const T& x, const T& y);
		
		inline void
		setX(const T& value);
		
		inline void
		setY(const T& value);
		
		inline void
		set(const T& x, const T& y);
		
		inline const T&
		getX() const;
		
		inline const T&
		getY() const;
		
		/**
		 * \brief	Convert between Point-objects with different base-types
		 */
		template<typename U>
		operator Point2D<U>() const;
	
	private:
		T x;
		T y;
		
	private:
		template<typename U>
		friend bool
		operator == (const Vector2D<U> &a, const Vector2D<U> &b);
		
		template<typename U>
		friend bool
		operator != (const Vector2D<U> &a, const Vector2D<U> &b);

		template<typename U>
		friend IOStream&
		operator <<(IOStream& s, const Vector2D<U>& c);
	};
	
	template<typename U>
	bool
	operator == (const Point2D<U> &a, const Point2D<U> &b);
	
	template<typename U>
	bool
	operator != (const Point2D<U> &a, const Point2D<U> &b);
	
	/**
	 * \brief	Stream operator for \b xpcc::Point2D<U>
	 * \ingroup	math
	 */
	template<typename U>
	IOStream&
	operator << (IOStream& s, const Point2D<U>& c);
}

#include "point_2d_impl.hpp"

#endif	// XPCC__POINT_2D_HPP
