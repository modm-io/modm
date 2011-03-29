// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
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

#ifndef XPCC_ATXMEGA__CLOCK_HPP
#define	XPCC_ATXMEGA__CLOCK_HPP

#include <xpcc/architecture/platform.hpp>

namespace xpcc
{
	// FIXME
	namespace atxmega
	{
		void
		enableExternalClock();
		
		/**
		 * 
		 * frequency:
		 * - OSC_FRQRANGE_04TO2_gc
		 * - OSC_FRQRANGE_2TO9_gc
		 * - OSC_FRQRANGE_9TO12_gc
		 * - OSC_FRQRANGE_12TO16_gc
		 * 
		 * startupTime:
		 * - OSC_XOSCSEL_XTAL_256CLK_gc
		 * - OSC_XOSCSEL_XTAL_1KCLK_gc
		 * - OSC_XOSCSEL_XTAL_16KCLK_gc
		 */
		void
		enableExternalOscillator(
				OSC_FRQRANGE_t frequency = OSC_FRQRANGE_12TO16_gc,
				OSC_XOSCSEL_t startupTime = OSC_XOSCSEL_XTAL_256CLK_gc);
		
		void
		enablePll();
		
		
		/**
		 * \brief	Select a new main clock source
		 * 
		 * Possible values:
		 * - internal 2 MHz  (CLK_SCLKSEL_RC2M_gc)
		 * - internal 32 MHz (CLK_SCLKSEL_RC32M_gc)
		 * - internal 32 kHz (CLK_SCLKSEL_RC32K_gc)
		 * - external clock or oscillator (CLK_SCLKSEL_XOSC_gc)
		 * - PLL (CLK_SCLKSEL_PLL_gc)
		 * 
		 * \warning	Make sure the clock source is stable before
		 * 			switching to it!
		 */
		void
		selectSystemClockSource(CLK_SCLKSEL_t source);
	}
}

#endif	// XPCC_ATXMEGA__CLOCK_HPP
