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

#include <xpcc/architecture/driver/atomic/lock.hpp>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/utils/dummy.hpp>

#include <xpcc/architecture.hpp>
//#include "device.h"  // TODO@Fabian: Why using an extra file and not rely on xpcc/architecture?
#include "systick_timer.hpp"

static xpcc::lpc11::InterruptHandler sysTickHandler = &xpcc::dummy;

extern "C" void
SysTick_Handler(void)
{
	xpcc::Clock::increment();
	sysTickHandler();
}

// ----------------------------------------------------------------------------
void
xpcc::lpc11::SysTickTimer::enable(uint32_t reload)
{
	// Lower systick interrupt priority to lowest level
	NVIC_SetPriority(SysTick_IRQn, 0xf);
	
	SysTick->LOAD = reload;
	SysTick->CTRL =
			SysTick_CTRL_CLKSOURCE_Msk |
			SysTick_CTRL_ENABLE_Msk |
			SysTick_CTRL_TICKINT_Msk;
}

void
xpcc::lpc11::SysTickTimer::disable()
{
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}

// ----------------------------------------------------------------------------
void
xpcc::lpc11::SysTickTimer::attachInterrupt(InterruptHandler handler)
{
	atomic::Lock lock;
	sysTickHandler = handler;
}

void
xpcc::lpc11::SysTickTimer::detachInterrupt()
{
	atomic::Lock lock;
	sysTickHandler = &xpcc::dummy;
}
