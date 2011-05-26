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

#ifndef	XPCC__LOCATION_2D_HPP
#define	XPCC__LOCATION_2D_HPP

#include <cmath>

#include <xpcc/io/iostream.hpp>

#include "angle.hpp"
#include "vector_2d.hpp"

namespace xpcc
{
	/**
	 * \brief	Location in a 2D coordinate system
	 * 
	 * This class is primarily used to describe the location of a robot
	 * on the field. The robot has a position (x- and y-coordinate) and a
	 * orientation (absolute angle in the range (-pi, pi)).
	 * 
	 * \ingroup	geometry
	 */
	template <typename T = int16_t>
	class Location2D
	{
	public:
		Location2D();
		
		Location2D(const Vector2D<T>& position, const float& orientation);
		
		Location2D(const T& x, const T& y, const float& orientation);
		
		inline const Vector2D<T>&
		getPosition() const;
		
		inline const T&
		getX() const;
		
		inline const T&
		getY() const;
		
		void
		setPosition(const Vector2D<T>& point);
		
		void
		setPosition(const T& x, const T& y);
		
		inline float
		getOrientation() const;
		
		void
		setOrientation(const float& phi);
		
		/// Add a position increment
		void
		move(const Location2D& diff);
		
		void
		move(const Vector2D<T>& diff);
		
		/**
		 * \brief	Add a increment only in x-direction
		 * 
		 * Our robots mostly use a differential drive with two driven wheels
		 * side by side, allowing the robot to move only in the drive direction
		 * (x-direction in the local coordinate system of the robot) and
		 * rotate. A movement perpendicular to the drive direction is
		 * impossible with a external force.
		 * 
		 * To estimate the position of the robot over time, we use odometry.
		 * Therefore it is necessary to add a lot small increments of
		 * movement over time.
		 * Because the y-component will always be zero, we created this
		 * method, which avoids unnecessary computations for the y-component
		 * and is therefore faster the the universal move-method.
		 * 
		 * \param	x		movement in x-direction
		 * \param	phi		rotation
		 */
		void
		move(T x, float phi);
		
		/// TODO
		Vector2D<T>
		translated(const Vector2D<T>& vector) const;
		
		/// Convert between Location-objects with different base-types
		template <typename U>
		Location2D<U>
		convert() const;
		
		bool
		operator == (const Location2D &other) const;
		
		bool
		operator != (const Location2D &other) const;
		
	private:
		template <typename U>
		friend IOStream&
		operator <<( IOStream&, const Location2D<U>&);
		
		Vector2D<T> position;
		float orientation;
	};
	
	// ------------------------------------------------------------------------
	// Global functions
	// ------------------------------------------------------------------------
	/**
	 * \brief	Stream operator to \b xpcc::Location<T>
	 * 
	 * \ingroup	geometry
	 */
	template<typename T>
	IOStream&
	operator << (IOStream& os, const Location2D<T>& l);
	
	// ------------------------------------------------------------------------
	// Declaration of specialized methods
	// ------------------------------------------------------------------------
	/*template<>
	bool
	Location2D<float>::operator == (const Location2D &other) const;
	
	template<>
	bool
	Location2D<double>::operator == (const Location2D &other) const;*/
}

#include "location_2d_impl.hpp"

#endif	// XPCC__LOCATION_2D_HPP
