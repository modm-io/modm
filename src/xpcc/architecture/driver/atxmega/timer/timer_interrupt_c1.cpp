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
/*
 * WARNING: This file is generated automatically, do not edit!
 * Please modify the corresponding *.in file instead and rebuild this file.
 */
// ----------------------------------------------------------------------------


#include <avr/interrupt.h>
#include "timer_interrupt_c1.hpp"

#ifdef TCC1

xpcc::TimerInterruptC1::F xpcc::TimerInterruptC1::overflow=xpcc::dummy;
xpcc::TimerInterruptC1::F xpcc::TimerInterruptC1::error=xpcc::dummy;
xpcc::TimerInterruptC1::F xpcc::TimerInterruptC1::cca=xpcc::dummy;
xpcc::TimerInterruptC1::F xpcc::TimerInterruptC1::ccb=xpcc::dummy;


ISR(TCC1_ERR_vect)
{
	xpcc::TimerInterruptC1::error();
}

ISR(TCC1_OVF_vect)
{
	xpcc::TimerInterruptC1::overflow();
}

ISR(TCC1_CCA_vect)
{
	xpcc::TimerInterruptC1::cca();
}

ISR(TCC1_CCB_vect)
{
	xpcc::TimerInterruptC1::ccb();
}



// specific configuration combinations
void
xpcc::TimerInterruptC1::setMsTimer(F function, uint8_t interval)
{
	setClockSource(TC_CLKSEL_DIV64_gc);
	attachOverflowInterrupt(TC_OVFINTLVL_MED_gc, function);
	TCC1_PER = (interval * F_CPU) / 64000l;
}

void
xpcc::TimerInterruptC1::attachCompareCaptureInterrupt(xpcc::timer::Channel channel, uint8_t level, F function)
{
	level <<= 2*channel;
	if (channel == xpcc::timer::CHANNELA) {
		attachCompareCaptureAInterrupt(static_cast<TC_CCAINTLVL_t>(level), function);
	}
	else if (channel == xpcc::timer::CHANNELB) {
		attachCompareCaptureBInterrupt(static_cast<TC_CCBINTLVL_t>(level), function);
	}
	
}

#endif	// TCC1