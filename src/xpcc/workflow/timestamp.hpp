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

#ifndef	XPCC__TIMESTAMP_HPP
#define	XPCC__TIMESTAMP_HPP

#include <stdint.h>

#include <xpcc/io/iostream.hpp>
#include <xpcc/utils/arithmetic_traits.hpp>

namespace xpcc
{
	/**
	 * \brief	Simple 16-bit timestamp
	 * 
	 * The timestap is calculated in milliseconds.
	 *
	 * \author	Fabian Greif
	 * \ingroup	workflow
	 */ 
	class Timestamp
	{
	public:
		typedef uint_fast16_t Type;
		
	public:
		///\param time in ms
		Timestamp(const uint_fast16_t time = 0) : 
			time(time)
		{
		}
		
		inline uint_fast16_t
		getTime() const
		{
			return this->time;
		}

		inline Timestamp
		operator + (const Timestamp& other) const
		{
			return Timestamp(time + other.time);
		}
		
		inline Timestamp
		operator - (const Timestamp& other) const
		{
			return Timestamp(time - other.time);
		}
		
		inline bool
		operator == (const Timestamp& other) const
		{
			return (time == other.time);
		}
		
		inline bool
		operator != (const Timestamp& other) const
		{
			return (time != other.time);
		}
		
		inline bool
		operator < (const Timestamp& other) const
		{
			return ((int_fast16_t) (time - other.time)) < 0;
		}
		
		inline bool
		operator > (const Timestamp& other) const
		{
			return ((int_fast16_t) (time - other.time)) > 0;
		}
		
		inline bool
		operator <= (const Timestamp& other) const
		{
			return ((int_fast16_t) (time - other.time)) <= 0;
		}
		
		inline bool
		operator >= (const Timestamp& other) const
		{
			return ((int_fast16_t) (time - other.time)) >= 0;
		}
	
	private:
		uint_fast16_t time;
	};
	
	// ------------------------------------------------------------------------
	inline IOStream&
	operator << (IOStream& os, const Timestamp& t)
	{
		os << t.getTime();
		return os;
	}
}

#endif	// XPCC__TIMESTAMP_HPP
