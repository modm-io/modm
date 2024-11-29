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

/// Implements the `std::latch` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/latch
/// @ingroup modm_processing_fiber
class latch
{
	latch(const latch&) = delete;
	latch& operator=(const latch&) = delete;

	using count_t = uint16_t;
	std::atomic<count_t> count;

public:
	constexpr explicit
	latch(std::ptrdiff_t expected)
	: count(expected) {}

	[[nodiscard]]
	static constexpr std::ptrdiff_t
	max() { return std::numeric_limits<count_t>::max(); }

	/// @note This function can be called from an interrupt.
	void inline
	count_down(count_t n=1)
	{
		// ensure we do not underflow the counter!
		count_t value = count.load(std::memory_order_relaxed);
		do if (value == 0) return;
		while (not count.compare_exchange_weak(value, value >= n ? value - n : 0,
					std::memory_order_acquire, std::memory_order_relaxed));
	}

	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	try_wait() const
	{
		return count.load(std::memory_order_relaxed) == 0;
	}

	void inline
	wait() const
	{
		while(not try_wait()) modm::this_fiber::yield();
	}

	void inline
	arrive_and_wait(std::ptrdiff_t n=1)
	{
		count_down(n);
		wait();
	}
};

}
