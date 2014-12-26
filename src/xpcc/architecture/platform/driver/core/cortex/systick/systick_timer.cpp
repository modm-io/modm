// coding: utf-8
/* Copyright (c) 2011-2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#include <xpcc/architecture/driver/atomic/lock.hpp>
#include <xpcc/architecture/driver/clock.hpp>
#include <xpcc/utils/dummy.hpp>

#include "../../../../device.hpp"
#include "systick_timer.hpp"


static xpcc::cortex::InterruptHandler sysTickHandler = &xpcc::dummy;

extern "C" void
SysTick_Handler(void)
{
	xpcc::Clock::increment();
	sysTickHandler();
}

// ----------------------------------------------------------------------------
void
xpcc::cortex::SysTickTimer::enable(uint32_t reload)
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
xpcc::cortex::SysTickTimer::disable()
{
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk;
}

// ----------------------------------------------------------------------------
void
xpcc::cortex::SysTickTimer::attachInterrupt(InterruptHandler handler)
{
	atomic::Lock lock;
	sysTickHandler = handler;
}

void
xpcc::cortex::SysTickTimer::detachInterrupt()
{
	atomic::Lock lock;
	sysTickHandler = &xpcc::dummy;
}
