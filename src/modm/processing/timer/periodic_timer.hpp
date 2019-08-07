/*
 * Copyright (c) 2009-2011, 2015, Fabian Greif
 * Copyright (c) 2010-2011, Georgi Grinshpun
 * Copyright (c) 2012, 2015, 2017, Niklas Hauser
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2017, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef MODM_PERIODIC_TIMER_HPP
#define MODM_PERIODIC_TIMER_HPP

#include "timeout.hpp"

namespace modm
{

/// Possible states of a timer
/// @ingroup	modm_processing_timer
enum class
PeriodicTimerState : uint8_t
{
	Stopped = 0,
	Expired = 0b010,
	Armed  = 0b100,
};

/**
 * Generic periodic software timeout class for variable timebase and timestamp width.
 *
 * @see		GenericTimeout
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
template< class Clock, typename TimestampType = modm::Timestamp >
class GenericPeriodicTimer
{
public:
	/// Create a stopped timer
	GenericPeriodicTimer();

	/// Create and start the timer
	GenericPeriodicTimer(TimestampType period);

	/// Restart the timer with the current period.
	void
	restart();

	/// Restart the timer with a new period value.
	void
	restart(TimestampType period);

	/// Stops the timer and sets isStopped() to `true`, and isExpired() to `false`.
	void
	stop();


	/// @return `true` exactly once during each period
	bool
	execute();


	/// @return the time until (positive time) or since (negative time) expiration, or 0 if stopped
	typename TimestampType::SignedType
	remaining() const;

	/// @return the set time period or zero if unset
	TimestampType
	getPeriod() const;


	/// @return the current state of the timer
	PeriodicTimerState
	getState() const;

	/// @return `true` if the timer has been stopped, `false` otherwise
	bool
	isStopped() const;

private:
	TimestampType period;
	GenericTimeout<Clock, TimestampType> timeout;
};

/**
 * Periodic software timer for up to 32 seconds with millisecond resolution.
 *
 * Extra care must be taken when not calling the isExpired() method
 * for more than 32 seconds. Due to an overflow in the implementation
 * this might add an additional delay of up to 32s ticks in the worst
 * case.
 * Always call restart() or restart(time) before reusing the timer
 * to avoid this behaviour.
 *
 * If you need a longer time period, use PeriodicTimer.
 *
 * @ingroup		modm_processing_timer
 */
using ShortPeriodicTimer = GenericPeriodicTimer< ::modm::Clock, ShortTimestamp>;

/// Periodic software timer for up to 24 days with millisecond resolution.
/// @ingroup	modm_processing_timer
using PeriodicTimer      = GenericPeriodicTimer< ::modm::Clock, Timestamp>;

}	// namespace

#include "periodic_timer_impl.hpp"

#endif // MODM_PERIODIC_TIMER_HPP
