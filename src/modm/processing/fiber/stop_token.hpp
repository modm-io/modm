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

#include <atomic>
#include <utility>

namespace modm::fiber
{

/// @ingroup modm_processing_fiber
/// @{

class stop_source;
class stop_token;

/**
 * Provides the implementation and memory storage for the stop request.
 * You must allocate this object yourself and derive the token and source then
 * from this object. This manual memory management differs from the `std`
 * interface.
 *
 * @warning The lifetime of this object must be longer or equal to the lifetime
 *          of the derived `stop_token` and `stop_source`.
 */
class stop_state
{
	std::atomic_bool requested{false};

public:
	/// @note This function can be called from an interrupt.
	[[nodiscard]]
	bool inline
	stop_requested() const
	{
		return requested.load(std::memory_order_relaxed);
	}

	/// @note This function can be called from an interrupt.
	bool inline
	request_stop()
	{
		return not requested.exchange(true, std::memory_order_relaxed);
	}

	/// @note This function can be called from an interrupt.
	void inline
	reset()
	{
		requested.store(false, std::memory_order_relaxed);
	}

	constexpr stop_source
	get_source();

	constexpr stop_token
	get_token();
};

/// Implements the `std::stop_token` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/stop_token
class stop_token
{
	friend class stop_state;
	const stop_state *state{nullptr};

	constexpr explicit stop_token(const stop_state *state) : state(state) {}
public:
	constexpr stop_token() = default;
	constexpr stop_token(const stop_token&) = default;
	constexpr stop_token(stop_token&&) = default;
	constexpr ~stop_token() = default;
	constexpr stop_token& operator=(const stop_token&) = default;
	constexpr stop_token& operator=(stop_token&&) = default;

	[[nodiscard]]
	bool inline
	stop_possible() const
	{
		return state;
	}

	[[nodiscard]]
	bool inline
	stop_requested() const
	{
		return stop_possible() and state->stop_requested();
	}

	void inline
	swap(stop_token& rhs)
	{
		std::swap(state, rhs.state);
	}

	[[nodiscard]]
	friend bool inline
	operator==(const stop_token& a, const stop_token& b)
	{
		return a.state == b.state;
	}

	friend void inline
	swap(stop_token& lhs, stop_token& rhs)
	{
		lhs.swap(rhs);
	}
};

/// Implements the `std::stop_source` interface for fibers.
/// @see https://en.cppreference.com/w/cpp/thread/stop_source
class stop_source
{
	friend class stop_state;
	stop_state *state{nullptr};
	explicit constexpr stop_source(stop_state *state) : state(state) {}

public:
	constexpr stop_source() = default;
	constexpr stop_source(const stop_source&) = default;
	constexpr stop_source(stop_source&&) = default;
	constexpr ~stop_source() = default;
	constexpr stop_source& operator=(const stop_source&) = default;
	constexpr stop_source& operator=(stop_source&&) = default;

	[[nodiscard]]
	constexpr bool
	stop_possible() const
	{
		return state;
	}

	[[nodiscard]]
	bool inline
	stop_requested() const
	{
		return stop_possible() and state->stop_requested();
	}

	bool inline
	request_stop()
	{
		return stop_possible() and state->request_stop();
	}

	[[nodiscard]]
	stop_token inline
	get_token() const
	{
		return state->get_token();
	}

	void inline
	swap(stop_source& rhs)
	{
		std::swap(state, rhs.state);
	}

	[[nodiscard]]
	friend bool inline
	operator==(const stop_source& a, const stop_source& b)
	{
		return a.state == b.state;
	}

	friend void inline
	swap(stop_source& lhs, stop_source& rhs)
	{
		lhs.swap(rhs);
	}
};

/// @cond
constexpr stop_source
stop_state::get_source()
{
	return stop_source{this};
}

constexpr stop_token
stop_state::get_token()
{
	return stop_token{this};
}
/// @endcond

/// @}

}
