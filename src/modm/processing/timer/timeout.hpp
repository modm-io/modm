/*
 * Copyright (c) 2009-2011, 2015, Fabian Greif
 * Copyright (c) 2011, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_TIMEOUT_HPP
#define MODM_TIMEOUT_HPP

#include <modm/architecture/interface/clock.hpp>

#include "timestamp.hpp"

namespace modm
{

/// Possible states of a timeout
/// @ingroup	modm_processing_timer
enum class
TimeoutState : uint8_t
{
	Stopped = 0,
	Expired = 0b010,
	Armed  = 0b100,
};

// forward declaration for friending
template< class Clock, class TimestampType >
class GenericPeriodicTimer;

/**
 * Generic software timeout class for variable timebase and timestamp width.
 *
 * @see		GenericPeriodicTimer
 *
 * @tparam	Clock
 * 		Used clock which inherits from modm::Clock, may have a variable timebase.
 * @tparam	TimestampType
 * 		Used timestamp which is compatible with the chosen Clock.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	modm_processing_timer
 */
template< class Clock, class TimestampType = modm::Timestamp >
class GenericTimeout
{
public:
	/// Creates a stopped timeout
	GenericTimeout();

	/// Create and start the timeout
	GenericTimeout(const TimestampType time);

	/// Set a new timeout value.
	void
	restart(TimestampType time);

	/// Stops the timer and sets isStopped() to `true`, and isExpired() to `false`.
	inline void
	stop();


	/// @return `true` exactly once, after the timeout expired
	bool
	execute();


	/// @return the time until (positive time) or since (negative time) expiration, or 0 if stopped
	inline typename TimestampType::SignedType
	remaining() const;


	/// @return the current state of the timeout
	TimeoutState
	getState() const;

	/// @return `true` if the timeout is stopped, `false` otherwise
	inline bool
	isStopped() const;

	/// @return `true` if the timeout has expired, `false` otherwise
	inline bool
	isExpired() const;

	/// @return `true` if the timeout is armed (not stopped and not expired), `false` otherwise
	inline bool
	isArmed() const;

private:
	inline bool
	checkExpiration() const;

private:
	enum
	InternalState : uint8_t
	{
		STOPPED  = 0b000,
		EXECUTED = 0b001,
		EXPIRED  = 0b010,
		ARMED    = 0b100,
		STATUS_MASK = 0b110
	};

	TimestampType endTime;
	mutable uint8_t state;

	friend class
	GenericPeriodicTimer<Clock, TimestampType>;
};

/**
 * Software timeout for up to 32 seconds with millisecond resolution.
 *
 * Extra care must be taken when not calling the isExpired() method
 * for more than 32 seconds. Due to an overflow in the implementation
 * this might add an additional delay of up to 32s ticks in the worst
 * case.
 * Always call restart(time) before reusing the timer to avoid this behaviour.
 *
 * If you need a longer time period, use Timeout.
 *
 * @ingroup		modm_processing_timer
 */
using ShortTimeout = GenericTimeout< ::modm::Clock, ShortTimestamp>;

/// Software timeout for up to 24 days with millisecond resolution.
/// @ingroup	modm_processing_timer
using Timeout      = GenericTimeout< ::modm::Clock, Timestamp>;

}	// namespace modm

#include "timeout_impl.hpp"

#endif // MODM_TIMEOUT_HPP
