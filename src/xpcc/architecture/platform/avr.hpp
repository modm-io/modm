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

#ifndef XPCC__AVR_HPP
#define XPCC__AVR_HPP

#include <avr/io.h>
#include <avr/interrupt.h>

/**
 * \ingroup		platform
 * \defgroup	avr		AVR
 * 
 * Tested under Linux with avr-gcc and under Windows with WinAVR (also avr-gcc).
 * Other compilers might work as well but are untested.
 */
namespace xpcc
{
	/**
	 * \brief	AVR specifiy functions
	 * \ingroup	avr
	 */
	namespace avr
	{
		/**
		 * \ingroup	avr
		 */
		static inline void
		enableInterrupts()
		{
			sei();
		}

		/**
		 * \ingroup	avr
		 */
		static inline void
		disableInterrupts()
		{
			cli();
		}
	}
}

#if defined XPCC__CPU_ATMEGA
#	include "avr/atmega.hpp"
#elif defined XPCC__CPU_ATXMEGA
#	include "avr/atxmega.hpp"
#elif defined XPCC__CPU_ATTINY
#	include "avr/attiny.hpp"
#endif

#endif	// XPCC__AVR_HPP
