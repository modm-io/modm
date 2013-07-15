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
		enum Frequency
		{
			HzDontCare  =         -1,
			Hz1         =          1,
			kHz1        = 1000 *  Hz1,
			MHz1        = 1000 * kHz1,
			MHz2        =    2 * MHz1,
			MHz4        =    4 * MHz1,
			MHz8        =    8 * MHz1,
			MHz16       =   16 * MHz1,
			MHz20       =   20 * MHz1,
			MHz24       =   24 * MHz1,
			MHz25       =   25 * MHz1,
			MHz26       =   26 * MHz1,
			MHz32       =   32 * MHz1,
			MHz48       =   48 * MHz1,
			MHz50       =   50 * MHz1,
			MHz64       =   64 * MHz1,
			MHz100      =  100 * MHz1,
			MHz120      =  120 * MHz1,
			MHz168      =  168 * MHz1,
			MHz192      =  192 * MHz1,
			MHz432      =  432 * MHz1,
		};

		static constexpr int
		MHz(int f) { return (f == HzDontCare)? HzDontCare : (f / MHz1); }
		static constexpr int
		kHz(int f) { return (f == HzDontCare)? HzDontCare : (f / kHz1); }
		static constexpr int
		Hz(int f)  { return (f == HzDontCare)? HzDontCare : (f / Hz1); }

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
