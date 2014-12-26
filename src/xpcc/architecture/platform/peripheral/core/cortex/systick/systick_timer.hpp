// coding: utf-8
/* Copyright (c) 2011-2013, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_CORTEX_SYSTICK_TIMER_HPP
#define XPCC_CORTEX_SYSTICK_TIMER_HPP

#include <stdint.h>

namespace xpcc
{

namespace cortex
{

typedef void (*InterruptHandler)(void);

/**
 * @brief		SysTick Timer
 * @ingroup		cortex
 */
class SysTickTimer
{
public:
	/**
	 * Enables the SysTick Timer to generate periodic events.
	 * 
	 * @param	reload
	 * 		Reload value to generate a 1000 Hz interrupt rate.
	 * 					
	 * 
	 * @warning	The SysTick Timer is used by default to increment
	 * 			xpcc::Clock, which is used by xpcc::Timeout and other
	 * 			similar processing classes.
	 * 			You must not increment the xpcc::Clock
	 * 			additionally somewhere else.
	 */
	static void
	enable(uint32_t reload = ((F_CPU / 1000) - 1));

	/**
	 * Disables SysTick Timer.
	 * 
	 * @warning	If the SysTick Timer is disabled xpcc::Clock is not
	 * 			incremented automatically. Workflow classes which
	 * 			rely on xpcc::Clock will not work if xpcc::Clock
	 * 			is not incremented.
	 */
	static void
	disable();

	/**
	 * Passed method will be called periodically on each event.
	 * Previously passed interrupt handler will be detached.
	 */
	static void
	attachInterrupt(InterruptHandler handler);

	/**
	 * Detaches previously attached interrupt handler.
	 */
	static void
	detachInterrupt();
};
}
}

#endif	//  XPCC_STM32_CORTEX_TIMER_HPP
