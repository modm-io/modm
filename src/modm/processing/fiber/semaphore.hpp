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
#include <limits>
#include <atomic>

namespace modm::fiber
{

/// @ingroup modm_processing_fiber
/// @{

/// Implements the `std::counting_semaphore` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/counting_semaphore
template< std::ptrdiff_t LeastMaxValue = 255 >
class counting_semaphore
{
	counting_semaphore(const counting_semaphore&) = delete;
	counting_semaphore& operator=(const counting_semaphore&) = delete;

	static_assert(LeastMaxValue <= uint16_t(-1), "counting_semaphore uses a 16-bit counter!");
	using count_t = std::conditional_t<(LeastMaxValue < 256), uint8_t, uint16_t>;
	std::atomic<count_t> count{};

public:
	constexpr explicit
	counting_semaphore(std::ptrdiff_t desired)
	: count(desired) {}

	[[nodiscard]]
	static constexpr std::ptrdiff_t
	max() { return std::numeric_limits<count_t>::max(); }

	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	try_acquire()
	{
		count_t current = count.load(std::memory_order_relaxed);
		do if (current == 0) return false;
		while(not count.compare_exchange_weak(current, current - 1,
						std::memory_order_acquire, std::memory_order_relaxed));
		return true;
	}

	void inline
	acquire()
	{
		while(not try_acquire()) modm::this_fiber::yield();
	}

	/// @note This function can be called from an interrupt.
	void inline
	release()
	{
		count.fetch_add(1, std::memory_order_release);
	}

	template< typename Rep, typename Period >
	[[nodiscard]]
	bool
	try_acquire_for(std::chrono::duration<Rep, Period> sleep_duration)
	{
		return this_fiber::poll_for(sleep_duration, [this]{ return try_acquire(); });
	}

	template< class Clock, class Duration >
	[[nodiscard]]
	bool
	try_acquire_until(std::chrono::time_point<Clock, Duration> sleep_time)
	{
		return this_fiber::poll_until(sleep_time, [this]{ return try_acquire(); });
	}
};

/// Implements the `std::binary_semaphore` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/counting_semaphore
using binary_semaphore = counting_semaphore<1>;

/// @}

}
