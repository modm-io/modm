/*
 * Copyright (c) 2024, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/fiber.hpp>
#include "stop_token.hpp"
#include <atomic>


namespace modm::fiber
{

/// @ingroup modm_processing_fiber
/// @{

enum class
cv_status
{
	no_timeout,
	timeout
};

/// Implements the `std::condition_variable_any` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/condition_variable
class condition_variable_any
{
	condition_variable_any(const condition_variable_any&) = delete;
	condition_variable_any& operator=(const condition_variable_any&) = delete;

	std::atomic<uint16_t> sequence{};

	const auto inline wait_on_sequence()
	{
		return [this, poll_sequence = sequence.load(std::memory_order_acquire)]
		{ return poll_sequence != sequence.load(std::memory_order_acquire); };
	}
public:
	constexpr condition_variable_any() = default;

	/// @note This function can be called from an interrupt.
	void inline
	notify_one()
	{
		sequence.fetch_add(1, std::memory_order_release);
	}

	/// @note This function can be called from an interrupt.
	void inline
	notify_any()
	{
		notify_one();
	}


	template< class Lock >
	void
	wait(Lock& lock)
	{
		lock.unlock();
		this_fiber::poll(wait_on_sequence());
		lock.lock();
	}

	template< class Lock, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	void
	wait(Lock& lock, Predicate&& pred)
	{
		while (not std::forward<Predicate>(pred)()) wait(lock);
	}

	template< class Lock, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait(Lock& lock, stop_token stoken, Predicate&& pred)
	{
		while (not stoken.stop_requested())
		{
			if (std::forward<Predicate>(pred)()) return true;
			wait(lock);
		}
		return std::forward<Predicate>(pred)();
	}


	template< class Lock, class Rep, class Period >
	cv_status
	wait_for(Lock& lock, std::chrono::duration<Rep, Period> rel_time)
	{
		lock.unlock();
		const bool result = this_fiber::poll_for(rel_time, wait_on_sequence());
		lock.lock();
		return result ? cv_status::no_timeout : cv_status::timeout;
	}

	template< class Lock, class Rep, class Period, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_for(Lock& lock, std::chrono::duration<Rep, Period> rel_time, Predicate&& pred)
	{
		while (not std::forward<Predicate>(pred)())
		{
			if (wait_for(lock, rel_time) == cv_status::timeout)
				return std::forward<Predicate>(pred)();
		}
		return true;
	}

	template< class Lock, class Rep, class Period, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_for(Lock& lock, stop_token stoken,
			 std::chrono::duration<Rep, Period> rel_time, Predicate&& pred)
	{
		while (not stoken.stop_requested())
		{
			if (std::forward<Predicate>(pred)()) return true;
			if (wait_for(lock, rel_time) == cv_status::timeout)
				return std::forward<Predicate>(pred)();
		}
		return std::forward<Predicate>(pred)();
	}


	template< class Lock, class Clock, class Duration >
	cv_status
	wait_until(Lock& lock, std::chrono::time_point<Clock, Duration> abs_time)
	{
		lock.unlock();
		const bool result = this_fiber::poll_until(abs_time, wait_on_sequence());
		lock.lock();
		return result ? cv_status::no_timeout : cv_status::timeout;
	}

	template< class Lock, class Clock, class Duration, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_until(Lock& lock, std::chrono::time_point<Clock, Duration> abs_time, Predicate&& pred)
	{
		while (not std::forward<Predicate>(pred)())
		{
			if (wait_until(lock, abs_time) == cv_status::timeout)
				return std::forward<Predicate>(pred)();
		}
		return true;
	}

	template< class Lock, class Clock, class Duration, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_until(Lock& lock, stop_token stoken,
			   std::chrono::time_point<Clock, Duration> abs_time, Predicate&& pred)
	{
		while (not stoken.stop_requested())
		{
			if (std::forward<Predicate>(pred)()) return true;
			if (wait_until(lock, abs_time) == cv_status::timeout)
				return std::forward<Predicate>(pred)();
		}
		return std::forward<Predicate>(pred)();
	}
};

/// There is no specialization for `std::unique_lock<fiber::mutex>`.
using condition_variable = condition_variable_any;

/// @}

}
