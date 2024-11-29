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

// Required to convince libstdc++ to enable scoped lock
#ifndef __cpp_lib_scoped_lock
#	define __cpp_lib_scoped_lock 201703L
#endif

#include "functions.hpp"
#include <modm/architecture/interface/atomic_lock.hpp>
#include <limits>
#include <atomic>
#include <mutex>

namespace modm::fiber
{

/// @ingroup modm_processing_fiber
/// @{

/// Implements the `std::mutex` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/mutex
class mutex
{
	mutex(const mutex&) = delete;
	mutex& operator=(const mutex&) = delete;

	std::atomic_bool locked{false};
public:
	constexpr mutex() = default;

	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	try_lock()
	{
		return not locked.exchange(true, std::memory_order_acquire);
	}

	void inline
	lock()
	{
		while(not try_lock()) modm::this_fiber::yield();
	}

	/// @note This function can be called from an interrupt.
	void inline
	unlock()
	{
		locked.store(false, std::memory_order_release);
	}
};

/// Implements the `std::timed_mutex` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/timed_mutex
class timed_mutex : public mutex
{
public:
	template< typename Rep, typename Period >
	[[nodiscard]]
	bool
	try_lock_for(std::chrono::duration<Rep, Period> sleep_duration)
	{
		return this_fiber::poll_for(sleep_duration, [this]{ return try_lock(); });
	}

	template< class Clock, class Duration >
	[[nodiscard]]
	bool
	try_lock_until(std::chrono::time_point<Clock, Duration> sleep_time)
	{
		return this_fiber::poll_until(sleep_time, [this]{ return try_lock(); });
	}
};

/// Implements the `std::recursive_mutex` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/recursive_mutex
class recursive_mutex
{
	recursive_mutex(const recursive_mutex&) = delete;
	recursive_mutex& operator=(const recursive_mutex&) = delete;
	using count_t = uint16_t;

	static constexpr fiber::id NoOwner{fiber::id(-1)};
	volatile fiber::id owner{NoOwner};
	static constexpr count_t countMax{count_t(-1)};
	volatile count_t count{1};

public:
	constexpr recursive_mutex() = default;

	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	try_lock()
	{
		const auto id = modm::this_fiber::get_id();
		modm::atomic::Lock _;
		if (owner == NoOwner) {
			owner = id;
			// count = 1; is implicit
			return true;
		}
		if (owner == id and count < countMax) {
			count++;
			return true;
		}
		return false;
	}

	void inline
	lock()
	{
		while(not try_lock()) modm::this_fiber::yield();
	}

	/// @note This function can be called from an interrupt.
	void inline
	unlock()
	{
		modm::atomic::Lock _;
		if (count > 1) count--;
		else {
			// count = 1; is implicit
			owner = NoOwner;
		}
	}
};

/// Implements the `std::recursive_timed_mutex` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/recursive_timed_mutex
class recursive_timed_mutex : public recursive_mutex
{
public:
	template< typename Rep, typename Period >
	[[nodiscard]]
	bool
	try_lock_for(std::chrono::duration<Rep, Period> sleep_duration)
	{
		return this_fiber::poll_for(sleep_duration, [this]{ return try_lock(); });
	}

	template< class Clock, class Duration >
	[[nodiscard]]
	bool
	try_lock_until(std::chrono::time_point<Clock, Duration> sleep_time)
	{
		return this_fiber::poll_until(sleep_time, [this]{ return try_lock(); });
	}
};



/// Implements the `std::once_flag` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/once_flag
class once_flag
{
	template<typename Callable, typename... Args>
	friend void call_once(once_flag&, Callable&&, Args&&...);

	once_flag(const once_flag&) = delete;
	once_flag& operator=(const once_flag&) = delete;

	std::atomic_flag state{};
	inline bool try_call() { return not state.test_and_set(std::memory_order_acquire); }

public:
	constexpr once_flag() = default;
};

/// Implements the `std::call_once` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/call_once
template<typename Callable, typename... Args>
void
call_once(once_flag& flag, Callable&& f, Args&&... args)
{
	if (flag.try_call()) std::forward<Callable>(f)(std::forward<Args>(args)...);
}

// Reuse the stdlibc++ implementation for the rest

/// @see https://en.cppreference.com/w/cpp/thread/lock_guard
using ::std::lock_guard;

/// @see https://en.cppreference.com/w/cpp/thread/scoped_lock
using ::std::scoped_lock;
/// @see https://en.cppreference.com/w/cpp/thread/unique_lock
using ::std::unique_lock;

/// @see https://en.cppreference.com/w/cpp/thread/lock_tag_t
using ::std::defer_lock_t;
/// @see https://en.cppreference.com/w/cpp/thread/lock_tag_t
using ::std::try_to_lock_t;
/// @see https://en.cppreference.com/w/cpp/thread/lock_tag_t
using ::std::adopt_lock_t;

/// @see https://en.cppreference.com/w/cpp/thread/lock_tag
using ::std::defer_lock;
/// @see https://en.cppreference.com/w/cpp/thread/lock_tag
using ::std::try_to_lock;
/// @see https://en.cppreference.com/w/cpp/thread/lock_tag
using ::std::adopt_lock;

/// @see https://en.cppreference.com/w/cpp/thread/try_lock
using ::std::try_lock;
/// @see https://en.cppreference.com/w/cpp/thread/lock
using ::std::lock;

/// @}

}
