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
 * $hello please remove me$
 */
// ----------------------------------------------------------------------------

#include "utils.hpp"
#include "clock.hpp"

// ----------------------------------------------------------------------------
void
xpcc::atxmega::enableExternalClock(OSC_FRQRANGE_t frequency)
{
	// select external clock with 8MHz as clock source
	OSC.XOSCCTRL = frequency | OSC_XOSCSEL_EXTCLK_gc;
	OSC.CTRL |= OSC_XOSCEN_bm;
	
	// wait for osc to become ready
	while ((OSC.STATUS & OSC_XOSCRDY_bm) == 0)
		;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::enableExternalOscillator(OSC_FRQRANGE_t frequency, OSC_XOSCSEL_t startupTime)
{
	// select external clock with 16MHz as clock source
	OSC.XOSCCTRL = frequency | startupTime;
	OSC.CTRL |= OSC_XOSCEN_bm;
	
	// wait for osc to become ready
	while ((OSC.STATUS & OSC_XOSCRDY_bm) == 0)
		;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::enablePll(OSC_PLLSRC_t source, uint8_t factor)
{
	// set PLL source to XOSC & factor to x4
	OSC.PLLCTRL = source | ((factor & 0x1f) << OSC_PLLFAC_gp);
	OSC.CTRL |= OSC_PLLEN_bm;		// enable PLL
	
	// wait for PLL to become ready
	while ((OSC.STATUS & OSC_PLLRDY_bm) == 0)
		;
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::selectSystemClockSource(CLK_SCLKSEL_t source)
{
	changeProtectedRegister(&CLK_CTRL, source);
}

// ----------------------------------------------------------------------------
void
xpcc::atxmega::setSystemClockPrescaler(CLK_PSADIV_t prescalerA,
		CLK_PSBCDIV_t prescalerBC)
{
	CLK.PSCTRL = prescalerA | prescalerBC;
}
