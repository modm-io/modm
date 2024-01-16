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
#include <type_traits>

namespace modm::this_fiber
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
	modm::fiber::Scheduler::instance().yield();
}

/// Returns the id of the current fiber
inline modm::fiber::id
get_id()
{
	return modm::fiber::Scheduler::instance().get_id();
}

/**
 * Yields the current fiber until the time duration has elapsed.
 * This functionality is a convenience wrapper around `modm::Timeout`
 * if duration ≥1ms or `modm::PreciseTimeout` if duration ≥1µs.
 * For nanosecond delays, use `modm::delay(ns)`.
 *
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep duration may be longer without any guarantee of an upper limit.
 */
template< typename Rep, typename Period >
void
sleep_for(std::chrono::duration<Rep, Period> sleep_duration)
{
	// Only choose the microsecond clock if necessary
	using Clock = std::conditional_t<
		std::is_convertible_v<std::chrono::duration<Rep, Period>,
							  std::chrono::duration<Rep, std::milli>>,
		modm::chrono::milli_clock, modm::chrono::micro_clock>;
	return sleep_until(Clock::now() + sleep_duration);
}

/**
 * Yields the current fiber until the sleep time has been reached.
 *
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep duration may be longer without any guarantee of an upper limit.
 */
template< class Clock, class Duration >
void
sleep_until(std::chrono::time_point<Clock, Duration> sleep_time)
{
	do modm::this_fiber::yield();
	while((sleep_time - Clock::now()) >= Duration(0));
}

/// @}

} // namespace modm::fiber
