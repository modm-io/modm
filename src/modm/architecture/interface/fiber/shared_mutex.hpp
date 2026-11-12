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
#include <atomic>
#include <shared_mutex>

namespace modm::fiber
{

/// @ingroup modm_architecture_fiber
/// @{

/// Implements the `std::shared_mutex` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/shared_mutex
class shared_mutex
{
	shared_mutex(const shared_mutex&) = delete;
	shared_mutex& operator=(const shared_mutex&) = delete;

	// Each reader counts the owner down from fiber::invalid_id, so the largest
	// values are shared owners. Fiber ids are addresses, 0 or interrupt numbers,
	// which are all far below these values.
	static constexpr fiber::id MaxSharedOwners{127};
	static constexpr fiber::id SharedOwnerLimit{fiber::invalid_id - MaxSharedOwners};
	std::atomic<fiber::id> owner{fiber::invalid_id};
public:
	constexpr shared_mutex() = default;

	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	try_lock()
	{
		const fiber::id new_owner = modm::this_fiber::get_id();
		fiber::id expected{fiber::invalid_id};
		return owner.compare_exchange_strong(expected, new_owner,
				std::memory_order_acquire, std::memory_order_relaxed);
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
		owner.store(fiber::invalid_id, std::memory_order_release);
	}

	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	try_lock_shared()
	{
		fiber::id current = owner.load(std::memory_order_relaxed);
		// owned by a fiber or by too many readers
		do if (current <= SharedOwnerLimit) return false;
		while(not owner.compare_exchange_weak(current, current - 1,
				std::memory_order_acquire, std::memory_order_relaxed));
		return true;
	}

	void inline
	lock_shared()
	{
		while(not try_lock_shared()) modm::this_fiber::yield();
	}

	/// @note This function can be called from an interrupt.
	void inline
	unlock_shared()
	{
		owner.fetch_add(1, std::memory_order_release);
	}
};

/// Implements the `std::shared_timed_mutex` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/shared_timed_mutex
class shared_timed_mutex : public shared_mutex
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

	template< typename Rep, typename Period >
	[[nodiscard]]
	bool
	try_lock_shared_for(std::chrono::duration<Rep, Period> sleep_duration)
	{
		return this_fiber::poll_for(sleep_duration, [this]{ return try_lock_shared(); });
	}

	template< class Clock, class Duration >
	[[nodiscard]]
	bool
	try_lock_shared_until(std::chrono::time_point<Clock, Duration> sleep_time)
	{
		return this_fiber::poll_until(sleep_time, [this]{ return try_lock_shared(); });
	}
};


// Reuse the stdlibc++ implementation
using ::std::shared_lock;

/// @}

}
