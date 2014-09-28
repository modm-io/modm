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
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC__CPU_BOARD2_SLAVE_HPP
	#error	"Don't include this file directly, use 'slave.hpp' instead"
#endif

// ----------------------------------------------------------------------------
template <typename Transmit, typename Receive>
bool
xpcc::CpuBoard2Slave<Transmit, Receive>::initialize()
{
	Leds::setOutput();
	Leds::write(0);
	
	enableExternalClock();
	
	Interconnect::initialize();
	
	for (uint8_t i = 0; i < 4; ++i)
	{
		Leds::write(0x0f);
		xpcc::delayMilliseconds(50);
		Leds::write(0x00);
		xpcc::delayMilliseconds(50);
	}
	
	return true;
}

// ----------------------------------------------------------------------------
template <typename Transmit, typename Receive>
void
xpcc::CpuBoard2Slave<Transmit, Receive>::enableExternalClock()
{
	// select external clock with 8MHz as clock source and set PLL source to XOSC & factor to x4
	xpcc::xmega::enableExternalClock(OSC_FRQRANGE_2TO9_gc);
	xpcc::xmega::enablePll(OSC_PLLSRC_XOSC_gc, 4);
	
	// set up prescalers (=1) and select PLL as clock source (4 x 8MHz)
	xpcc::xmega::setSystemClockPrescaler();
	xpcc::xmega::selectSystemClockSource(CLK_SCLKSEL_PLL_gc);
}
