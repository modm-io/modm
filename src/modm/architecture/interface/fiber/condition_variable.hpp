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

/// @ingroup modm_architecture_fiber
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

	// Must be called before unlocking, otherwise a notification between
	// unlocking and calling this function is lost.
	const auto inline wait_on_sequence()
	{
		return [this, poll_sequence = sequence.load(std::memory_order_acquire)]
		{ return poll_sequence != sequence.load(std::memory_order_acquire); };
	}

	// Waits until the predicate is true after a notification or a stop
	// request, or until poll() gives up. The sequence is read before every
	// evaluation of the predicate, so that a notification right after it,
	// e.g. from an interrupt that cannot hold the lock, is not lost.
	template< class Lock, class Predicate, class Poll >
	bool
	wait_predicate(Lock& lock, Predicate&& pred, stop_token stoken, Poll&& poll)
	{
		uint16_t seen = sequence.load(std::memory_order_acquire);
		if (pred()) return true;
		if (stoken.stop_requested()) return false;
		bool locked{false};
		lock.unlock();
		(void) poll([&]
		{
			if (seen == sequence.load(std::memory_order_acquire) and
				not stoken.stop_requested()) return false;
			lock.lock();
			seen = sequence.load(std::memory_order_acquire);
			if (pred() or stoken.stop_requested()) return locked = true;
			lock.unlock();
			return false;
		});
		if (not locked) lock.lock();
		return pred();
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
	notify_all()
	{
		notify_one();
	}

	[[deprecated("Use notify_all() instead!")]]
	void inline
	notify_any() // DEPRECATED: 2027q4
	{
		notify_one();
	}


	template< class Lock >
	void
	wait(Lock& lock)
	{
		const auto notified = wait_on_sequence();
		lock.unlock();
		this_fiber::poll(notified);
		lock.lock();
	}

	template< class Lock, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	void
	wait(Lock& lock, Predicate&& pred)
	{
		(void) wait_predicate(lock, pred, {}, [](auto&& c) { this_fiber::poll(c); return true; });
	}

	template< class Lock, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait(Lock& lock, stop_token stoken, Predicate&& pred)
	{
		return wait_predicate(lock, pred, stoken, [](auto&& c) { this_fiber::poll(c); return true; });
	}


	template< class Lock, class Rep, class Period >
	cv_status
	wait_for(Lock& lock, std::chrono::duration<Rep, Period> rel_time)
	{
		const auto notified = wait_on_sequence();
		lock.unlock();
		const bool result = this_fiber::poll_for(rel_time, notified);
		lock.lock();
		return result ? cv_status::no_timeout : cv_status::timeout;
	}

	template< class Lock, class Rep, class Period, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_for(Lock& lock, std::chrono::duration<Rep, Period> rel_time, Predicate&& pred)
	{
		return wait_predicate(lock, pred, {},
				[&](auto&& c) { return this_fiber::poll_for(rel_time, c); });
	}

	template< class Lock, class Rep, class Period, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_for(Lock& lock, stop_token stoken,
			 std::chrono::duration<Rep, Period> rel_time, Predicate&& pred)
	{
		return wait_predicate(lock, pred, stoken,
				[&](auto&& c) { return this_fiber::poll_for(rel_time, c); });
	}


	template< class Lock, class Clock, class Duration >
	cv_status
	wait_until(Lock& lock, std::chrono::time_point<Clock, Duration> abs_time)
	{
		const auto notified = wait_on_sequence();
		lock.unlock();
		const bool result = this_fiber::poll_until(abs_time, notified);
		lock.lock();
		return result ? cv_status::no_timeout : cv_status::timeout;
	}

	template< class Lock, class Clock, class Duration, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_until(Lock& lock, std::chrono::time_point<Clock, Duration> abs_time, Predicate&& pred)
	{
		return wait_predicate(lock, pred, {},
				[&](auto&& c) { return this_fiber::poll_until(abs_time, c); });
	}

	template< class Lock, class Clock, class Duration, class Predicate >
	requires requires { std::is_invocable_r_v<bool, Predicate, void>; }
	bool
	wait_until(Lock& lock, stop_token stoken,
			   std::chrono::time_point<Clock, Duration> abs_time, Predicate&& pred)
	{
		return wait_predicate(lock, pred, stoken,
				[&](auto&& c) { return this_fiber::poll_until(abs_time, c); });
	}
};

/// There is no specialization for `std::unique_lock<fiber::mutex>`.
using condition_variable = condition_variable_any;

/// @}

}
