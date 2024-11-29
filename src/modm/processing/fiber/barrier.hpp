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

namespace modm::fiber
{

/// Implements the `std::barrier` interface for fibers.
/// @warning This implementation is not interrupt-safe!
/// @see https://en.cppreference.com/w/cpp/thread/barrier
/// @ingroup modm_processing_fiber
template< class CompletionFunction = decltype([]{}) >
class barrier
{
	barrier(const barrier&) = delete;
	barrier& operator=(const barrier&) = delete;
	using count_t = uint16_t;

	const CompletionFunction completion;
	count_t expected;
	count_t count;
	count_t sequence{};

public:
	using arrival_token = count_t;

	constexpr explicit
	barrier(std::ptrdiff_t expected, CompletionFunction f = CompletionFunction())
	: completion(std::move(f)), expected(expected), count(expected) {}

	[[nodiscard]]
	static constexpr std::ptrdiff_t
	max() { return std::numeric_limits<count_t>::max(); }

	[[nodiscard]]
	arrival_token
	arrive(count_t n=1)
	{
		count_t last_arrival{sequence};
		if (n < count) count -= n;
		else
		{
			count = expected;
			sequence++;
			completion();
		}
		return last_arrival;
	}

	void
	wait(arrival_token arrival) const
	{
		while (arrival == sequence) modm::this_fiber::yield();
	}

	void
	arrive_and_wait()
	{
		wait(arrive());
	}

	void
	arrive_and_drop()
	{
		if (expected) expected--;
		(void) arrive();
	}
};

}
