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

#ifndef XPCC__GPIO_HPP
#define XPCC__GPIO_HPP

#include <xpcc/utils/misc.hpp>

namespace xpcc
{
	/**
	 * @ingroup	hal
	 * @headerfile	<xpcc/hal/peripheral/atxmega/gpio.hpp>
	 */
	namespace gpio
	{
		/**
		 * @brief	Dummy implementation of an I/O pin
		 * 
		 */
		class Unused
		{
		public:
			ALWAYS_INLINE static void
			output()
			{
			}
			
			ALWAYS_INLINE static void
			input()
			{
			}
			
			ALWAYS_INLINE static void
			set()
			{
			}
			
			ALWAYS_INLINE static void
			set(bool status)
			{
				(void) status;
			}
		
			ALWAYS_INLINE static void
			reset()
			{
			}
			
			ALWAYS_INLINE static bool
			get()
			{
				return false;
			}
		};
	}
}

#if defined(__AVR__)

	#if defined(__AVR_XMEGA__)
		#include "atxmega/gpio.hpp"
	#elif defined(__AVR_MEGA__)
		#include "atmega/gpio.hpp"
	#else
		#error	"Unknown AVR target!"
	#endif

#else

	#error "No definitions for this target available!"

#endif

#endif // XPCC__GPIO_HPP
