/*
 * Copyright (c) 2009-2011, 2015, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2015, 2020, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "timestamp.hpp"
#include <modm/math/utils/arithmetic_traits.hpp>
#include <modm/architecture/interface/clock.hpp>
#include <modm/architecture/interface/assert.hpp>

namespace modm
{

/// Possible states of a timeout
/// @ingroup	modm_processing_timer
enum class
TimerState : uint8_t
{
	Stopped = 0b001,
	Expired = 0b010,
	Armed   = 0b100,
};

// forward declaration for friending
template< class Clock, class Duration >
class GenericPeriodicTimer;

/**
 * Generic software timeout class for variable timebase and timestamp width.
 *
 * @see		GenericPeriodicTimer
 *
 * @tparam	Clock
 * 		Used clock which inherits from modm::Clock, may have a variable timebase.
 * @tparam	DurationType
 * 		Used timestamp which is compatible with the chosen Clock.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	modm_processing_timer
 */
template< class Clock, class Duration >
class GenericTimeout
{
public:
	using clock = Clock;
	using period = typename Duration::period;
	using rep = typename Duration::rep;
	using time_point = std::chrono::time_point<Clock, Duration>;
	using duration = Duration;
	using wide_signed_duration = std::chrono::duration<
							modm::WideType<std::make_signed_t<rep>>, period>;

	/// Create a stopped timeout
	GenericTimeout() = default;

	/// Create and start the timeout
	template< typename Rep, typename Period >
	GenericTimeout(std::chrono::duration<Rep, Period> interval);

	/// Restart the timer with the current timeout.
	void restart();
	/// Set a new timeout value.
	template< typename Rep, typename Period >
	void restart(std::chrono::duration<Rep, Period> interval);

	/// Stops the timer and sets isStopped() to `true`, and isExpired() to `false`.
	void
	stop();

	/// @return the time until (positive time) or since (negative time) expiration, or 0 if stopped
	wide_signed_duration
	remaining() const;

	/// @return the currently set interval
	duration
	interval() const;

	/// @return the current state of the timeout
	TimerState
	state() const;

	/// @return `true` if the timeout is stopped, `false` otherwise
	bool
	isStopped() const;

	/// @return `true` if the timeout has expired, `false` otherwise
	bool
	isExpired() const;

	/// @return `true` if the timeout is armed (not stopped and not expired), `false` otherwise
	bool
	isArmed() const;

	/// @return `true` exactly once, after the timeout expired
	bool
	execute();

	/// @cond
	[[deprecated("Use `std::chrono::{milli,micro}seconds` for interval instead!")]]
	GenericTimeout(rep interval) :
		GenericTimeout(duration(interval)) {}
	[[deprecated("Use `std::chrono::{milli,micro}seconds` for interval instead!")]]
	void restart(rep interval)
	{ restart(duration(interval)); }
	[[deprecated("Use `Timeout.state()` instead!")]]
	TimerState getState() const { return state(); }
	/// @endcond

protected:
	bool
	checkExpiration() const;

	time_point
	now() const;

	enum
	InternalState : uint8_t
	{
		STOPPED  = int(TimerState::Stopped),
		EXPIRED  = int(TimerState::Expired),
		ARMED    = int(TimerState::Armed),
		EXECUTED = 0b1000,
		STATUS_MASK = (EXPIRED | ARMED | STOPPED)
	};

	time_point _start{duration{0}};
	duration _interval{0};
	mutable uint8_t _state{STOPPED};

	friend class
	GenericPeriodicTimer<Clock, Duration>;
};

/**
 * Software timeout for up to 65 seconds with millisecond resolution.
 *
 * Extra care must be taken when not calling the isExpired() method
 * for more than 65 seconds. Due to an overflow in the implementation
 * this might add an additional delay of up to 65s ticks in the worst
 * case.
 * Always call restart() before reusing the timer to avoid this behaviour.
 *
 * If you need a longer time period, use Timeout.
 *
 * @ingroup		modm_processing_timer
 */
using        ShortTimeout = GenericTimeout< Clock, ShortDuration >;

/// Software timeout for up to 49 days with millisecond resolution.
/// @ingroup	modm_processing_timer
using             Timeout = GenericTimeout< Clock, Duration >;

/// Software timeout for up to 65 milliseconds with microsecond resolution.
/// @ingroup	modm_processing_timer
using ShortPreciseTimeout = GenericTimeout< PreciseClock, ShortPreciseDuration >;

/// Software timeout for up to 71 minutes with microsecond resolution.
/// @ingroup	modm_processing_timer
using      PreciseTimeout = GenericTimeout< PreciseClock, PreciseDuration >;

/// @cond
using TimeoutState [[deprecated("Use `modm::TimerState` instead!")]] = TimerState;
/// @endcond

}	// namespace modm

#include "timeout_impl.hpp"
