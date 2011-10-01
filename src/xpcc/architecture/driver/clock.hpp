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

#ifndef	XPCC__CLOCK_HPP
#define	XPCC__CLOCK_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/workflow/timestamp.hpp>

namespace xpcc
{
	/**
	 * \brief	Internal timer
	 * 
	 * This class is implemented using \c gettimeofday() from <sys/time.h> for
	 * any Unix-OS. 
	 * 
	 * For the AVRs targets the user has to use the increment() method to
	 * generate a suitable timebase, preferably by incrementing the time
	 * value inside a timer interrupt function.
	 * 
	 * Example:
	 * \code
	 * // Interrupt every 1ms
	 * ISR(TIMER)
	 * {
	 *     xpcc::Clock::increment();
	 * }
	 * \endcode
	 * 
	 * \ingroup	architecture
	 */ 
	class Clock
	{
	public:
		/**
		 * \brief	Get the current time
		 * 
		 * Provides an atomic access to the current time
		 */
		static Timestamp
		now();
		
#if !defined(XPCC__OS_STRING)
		/// \brief	Set the current time
		static inline void
		increment(uint_fast16_t step = 1)
		{
			time += step;
		}
#endif
		
	protected:
		static uint_fast16_t time;
	};
	
	/**
	 * \brief	Internal timer
	 * 
	 * This class is implemented using \c gettimeofday() from <sys/time.h> for
	 * any Unix-OS. 
	 * 
	 * For the AVRs targets the user has to use the increment() method to
	 * generate a suitable timebase, preferably by incrementing the time
	 * value inside a timer interrupt function.
	 * 
	 * Example:
	 * \code
	 * // Interrupt every 1ms
	 * ISR(TIMER)
	 * {
	 *     xpcc::Clock::increment();
	 * }
	 * \endcode
	 * 
	 * \ingroup	architecture
	 */
	class FlexClock
	{
	public:
		/**
		 * \brief	Get the current time
		 * 
		 * Provides an atomic access to the current time
		 */
		static Timestamp
		now();
		
#if !defined(XPCC__OS_STRING)
		/// \brief	Set the current time
		static inline void
		increment(uint_fast16_t step = 1)
		{
			time += step;
		}
#endif
		
	protected:
		static uint_fast16_t time;
	};
}

#endif	// XPCC__CLOCK_HPP
