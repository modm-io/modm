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

#include <modm/architecture/interface/clock.hpp>
#include <utility>
#include <type_traits>

namespace modm::fiber
{

/// Identifier of a fiber task.
/// @ingroup modm_architecture_fiber
using id = uintptr_t;

} // namespace modm::fiber

namespace modm::this_fiber
{

/// @ingroup modm_architecture_fiber
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
void
yield();

/// Returns the id of the current fiber
modm::fiber::id
get_id();

/// Yields the current fiber until `bool condition()` returns true.
/// @warning If `bool condition()` is true on first call, no yield is performed!
template< class Function >
requires requires { std::is_invocable_r_v<bool, Function, void>; }
void
poll(Function &&condition)
{
	while(not std::forward<Function>(condition)())
		modm::this_fiber::yield();
}

/**
 * Yields the current fiber until `bool condition()` returns true or the time
 * duration has elapsed.
 *
 * @warning If `bool condition()` is true on first call, no yield is performed!
 *
 * @returns `true` if the condition was met, `false` if the time duration has
 *          elapsed.
 *
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep duration may be longer without any guarantee of an upper limit.
 */
template< class Rep, class Period, class Function >
requires requires { std::is_invocable_r_v<bool, Function, void>; }
[[nodiscard]]
bool
poll_for(std::chrono::duration<Rep, Period> sleep_duration, Function &&condition)
{
	if (std::forward<Function>(condition)()) return true;

	// Only choose the microsecond clock if necessary
	using Clock = std::conditional_t<
		std::is_convertible_v<std::chrono::duration<Rep, Period>,
							  std::chrono::duration<Rep, std::milli>>,
		modm::chrono::milli_clock, modm::chrono::micro_clock>;

	// Ensure the sleep duration is rounded up to the next full clock tick
	const auto clock_sleep_duration(
			std::chrono::ceil<typename Clock::duration>(sleep_duration));

	const auto start = Clock::now();
	do {
		modm::this_fiber::yield();
		if (std::forward<Function>(condition)()) return true;
	}
	while((Clock::now() - start) < clock_sleep_duration);
	return false;
}

/**
 * Yields the current fiber until `bool condition()` returns true or the sleep
 * time has been reached.
 *
 * @warning If `bool condition()` is true on first call, no yield is performed!
 *
 * @returns `true` if the condition was met, `false` if the sleep time has
 *          elapsed.
 *
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep duration may be longer without any guarantee of an upper limit.
 */
template< class Clock, class Duration, class Function >
requires requires { std::is_invocable_r_v<bool, Function, void>; }
[[nodiscard]]
bool
poll_until(std::chrono::time_point<Clock, Duration> sleep_time, Function &&condition)
{
	if (std::forward<Function>(condition)()) return true;

	const auto start = Clock::now();
	const auto sleep_duration = sleep_time - start;
	do {
		modm::this_fiber::yield();
		if (std::forward<Function>(condition)()) return true;
	}
	while((Clock::now() - start) < sleep_duration);
	return false;
}

/**
 * Yields the current fiber until the time duration has elapsed.
 *
 * @note For nanosecond delays, use `modm::delay(ns)`.
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep duration may be longer without any guarantee of an upper limit.
 * @see https://en.cppreference.com/w/cpp/thread/sleep_for
 */
template< class Rep, class Period >
void
sleep_for(std::chrono::duration<Rep, Period> sleep_duration)
{
	(void) poll_for(sleep_duration, []{ return false; });
}

/**
 * Yields the current fiber until the sleep time has been reached.
 *
 * @note Due to the overhead of `yield()` and the scheduling other fibers, the
 *       sleep duration may be longer without any guarantee of an upper limit.
 * @see https://en.cppreference.com/w/cpp/thread/sleep_until
 */
template< class Clock, class Duration >
void
sleep_until(std::chrono::time_point<Clock, Duration> sleep_time)
{
	(void) poll_until(sleep_time, []{ return false; });
}

/// @}

} // namespace modm::this_fiber
