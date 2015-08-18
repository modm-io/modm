// coding: utf-8
/* Copyright (c) 2011, Roboterclub Aachen e.V.
* All Rights Reserved.
*
* The file is part of the xpcc library and is released under the 3-clause BSD
* license. See the file `LICENSE` for the full license governing this code.
*/
// ----------------------------------------------------------------------------

#ifndef XPCC_CORTEX_SYSTICK_TIMER_HPP
#define XPCC_CORTEX_SYSTICK_TIMER_HPP

#include <stdint.h>
#include <xpcc/architecture/interface.hpp>

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
	template< class SystemClock, uint16_t rate = 1000,
			uint16_t tolerance = xpcc::Tolerance::ZeroPercent >
	static void
	initialize()
	{
		constexpr float desired = float(SystemClock::Frequency) / rate;

		// calculate the possible rates above and below the requested rate
		constexpr uint32_t rate_lower = SystemClock::Frequency / std::ceil(desired);
		constexpr uint32_t rate_upper = SystemClock::Frequency / std::floor(desired);

		// calculate the half-point between the upper and lower rate
		constexpr uint32_t rate_middle = (rate_upper + rate_lower) / 2;
		// decide which reload value is closer to a possible rate
		constexpr uint32_t reload = (rate < rate_middle) ? std::ceil(desired) : std::floor(desired);

		// check if within rate tolerance
		constexpr uint32_t generated_rate = SystemClock::Frequency / reload;
		Peripheral::assertBaudrateInTolerance<
				/* nearest possible value */ generated_rate,
				/* desired = */ rate,
				tolerance >();

		enable(reload - 1);
	}

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
	attachInterruptHandler(InterruptHandler handler);

	/**
	 * Detaches previously attached interrupt handler.
	 */
	static void
	detachInterruptHandler();

private:
	static void
	enable(uint32_t reload);
};

}

}

#endif	//  XPCC_STM32_CORTEX_TIMER_HPP
