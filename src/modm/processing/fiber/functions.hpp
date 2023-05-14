/*
 * Copyright (c) 2020, Erik Henriksson
 * Copyright (c) 2022, Andrey Kunitsyn
 * Copyright (c) 2023, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include "scheduler.hpp"
#include <modm/processing/timer.hpp>

namespace modm::fiber
{

/// @ingroup modm_processing_fiber
/// @{

/**
 * Calls into the currently active scheduler to jump to the next fiber.
 * Local control flow resumes by returning from this function.
 *
 * @note If called while no scheduler is active, or only one fiber is currently
 *       running, this function returns immediately without performing a jump.
 *       This can cause deadlocks if the calling fiber waits on progress made
 *       outside of it.
 *
 * @warning Be careful to yield inside long running loops or when busy-waiting.
 *          These are cooperative fibers, so the scheduler will not preempt
 *          fibers, and you may starve other fibers of processing time.
 *          If you cannot guarantee correct timings, consider using an RTOS.
 */
inline void
yield()
{
	Scheduler::instance().yield();
}

/**
 * Yields the current fiber until the time interval has elapsed.
 * This functionality is a convenience wrapper around `modm::Timeout`
 * if interval ≥1ms or `modm::PreciseTimeout` if interval ≥1µs.
 * For nanosecond delays, use `modm::delay(ns)`.
 *
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep interval may be longer without any guarantee of an upper limit.
 */
template< typename Rep, typename Period >
void
sleep(std::chrono::duration<Rep, Period> interval)
{
	// Only choose the microsecond clock if necessary
	using TimeoutType = std::conditional_t<
		std::is_convertible_v<std::chrono::duration<Rep, Period>,
							  std::chrono::duration<Rep, std::milli>>,
		modm::GenericTimeout< modm::chrono::milli_clock, modm::chrono::milli_clock::duration>,
		modm::GenericTimeout< modm::chrono::micro_clock, modm::chrono::micro_clock::duration>
	>;

	TimeoutType timeout(interval);
	while(not timeout.isExpired())
		modm::fiber::yield();
}

/// @}

} // namespace modm::fiber
