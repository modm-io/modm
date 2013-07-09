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
			HzDontCare  =         -1,
			Hz          =          1,
			Hz1         =          1,
			kHz         = 1000 *  Hz,
			kHz1        = 1000 *  Hz,
			MHz         = 1000 * kHz,
			MHz1        =    1 * MHz,
			MHz2        =    2 * MHz,
			MHz4        =    4 * MHz,
			MHz8        =    8 * MHz,
			MHz16       =   16 * MHz,
			MHz20       =   20 * MHz,
			MHz24       =   24 * MHz,
			MHz25       =   25 * MHz,
			MHz26       =   26 * MHz,
			MHz32       =   32 * MHz,
			MHz48       =   48 * MHz,
			MHz50       =   50 * MHz,
			MHz64       =   64 * MHz,
			MHz100      =  100 * MHz,
			MHz120      =  120 * MHz,
			MHz168      =  168 * MHz,
			MHz192      =  192 * MHz,
			MHz432      =  432 * MHz,
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
