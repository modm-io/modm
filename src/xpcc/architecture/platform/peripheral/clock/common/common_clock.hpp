// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
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

#ifndef XPCC__COMMON_CLOCK_HPP
#define XPCC__COMMON_CLOCK_HPP

namespace xpcc
{
	namespace clock
	{
		/**
		 * Frequencies enum to make code more readable for common
		 * frequencies (in kHz).
		 */
		enum Frequencies
		{
			HzDontCare  =     -1,
			kHz1        =      1,
			MHz1        =   1000,
			MHz2        =   2000,
			MHz8        =   8000,
			MHz16       =  16000,
			MHz20       =  20000,
			MHz24       =  24000,
			MHz25       =  25000,
			MHz32       =  32000,
			MHz48       =  48000,
			MHz50       =  50000,
			MHz64       =  64000,
			MHz100      = 100000,
			MHz120      = 120000,
			MHz168      = 168000,
			MHz192      = 192000,
			MHz432      = 432000,
		};

		/**
		 * Errors that might prevent using a certain clock configuration
		 */
		enum class StartupError
		{
			None,				// all went well
			InternalClock,		// was not able to start the internal clock
			ExternalClock,		// was not able to start the external clock
			ExternalOscillator,	// was not able to start the external oscialltor
			Pll,				// was not able to start the PLL
			SystemClock,		// was not able to switch system clock input
		};
	}
}

#endif	/* XPCC__COMMON_CLOCK_HPP */
