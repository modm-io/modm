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

#ifndef	XPCC__LOCATION_HPP
#define	XPCC__LOCATION_HPP

#include <xpcc/communication/io/iostream.hpp>

#include "angle.hpp"
#include "position.hpp"

namespace xpcc
{
	/**
	 * \brief		Location
	 * 
	 * \ingroup	math
	 */
	template <typename T = int16_t>
	class Location
	{
	public:
		typedef ::xpcc::Position<T> Position;

		Location();

		Location(const Position& position, const Angle& phi);

		//Location(const Position& position, float phi);

		Location(const T& x, const T& y, const Angle& phi);

		//Location(const T& x, const T& y, float phi);

		/// Add a position increment
		void
		update(Location& diff);
		
		/// Add a increment only in x-direction
		void
		update(T x, Angle& phi);
		
		inline const Position&
		getPosition() const;
		
		void
		setPosition(const Position& position);
		
		void
		setPosition(const T& x, const T& y);
		
		inline const Angle&
		getAngle() const;
		
		void
		setAngle(const Angle& phi);
		
		/// Convert between Location-objects with different base-types
		template<typename U>
		operator Location<U>() const;
		
	private:
		template <typename U>
		friend IOStream&
		operator <<( IOStream&, const Location<U>&);
		
		Position position;
		Angle phi;
	};

	/**
	 * \brief	Stream operator to \b xpcc::Location<T>
	 *
	 * \ingroup	math
	 */
	template<typename T>
	IOStream&
	operator<<(IOStream& os, const Location<T>& l);
}

#include "location_impl.hpp"

#endif	// XPCC__LOCATION_HPP
