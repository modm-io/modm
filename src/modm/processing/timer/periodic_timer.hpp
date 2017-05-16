/*
 * Copyright (c) 2009-2011, 2015, Fabian Greif
 * Copyright (c) 2010-2011, Georgi Grinshpun
 * Copyright (c) 2012, 2015, Niklas Hauser
 * Copyright (c) 2013-2014, Kevin Läufer
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
/// @ingroup	software_timer
enum class
PeriodicTimerState : uint8_t
{
	Stopped = 0,
	Expired = 0b010,
	Armed  = 0b100,
};

/**
 * Generic software timeout class for variable timebase and timestamp width.
 *
 * This class allows for periodic code execution
 *
 * Its logic can be described by the following annotated waveform:
 *
 * - C: Constructor
 * - S: (Re-)Start timer
 * - I: Period interval
 * - H: Code handler (`execute()` returned `true`)
 * - P: Stop timer
 *
@verbatim
Event:    C         IH        I         I  H      I  S    IH   I    IH  P
                     _         _____________       __      _    ______
Expired:  __________/ \_______/             \_____/  \____/ \__/      \____...
          __________   _______               _____    ____   __        _
Armed:              \_/       \_____________/     \__/    \_/  \______/ \__...
                                                                         __
Stopped:  ______________________________________________________________/  ...

                     _                     _               _         _
Handle:   __________/ \___________________/ \_____________/ \_______/ \____...

Remaining:     +    |0|   +   |     -     |0|  +  | -|  + |0| +| -  |0|+| 0
@endverbatim
 *
 * If you want to execute code once per period interval, poll the
 * `execute()` method, which returns `true` exactly once after expiration.
 *
 * @code
 * if (timer.execute())
 * {
 *     // periodically called once
 *     Led::toggle();
 * }
 * @endcode
 *
 * Be aware, however, that since this method is polled, it cannot execute
 * exactly at the time of expiration, but some time after expiration, as
 * indicated in the above waveform graph.
 * If one or several periods are missed when polling `execute()`, these
 * code executions are discarded and will not be cought up.
 * Instead, `execute()` returns `true` once and then reschedules itself
 * for the next period, without any period skewing.
 *
 * @warning	Never use this class when a precise timebase is needed!
 *
 * Notice, that the `PeriodicTimerState::Expired` is reset to
 * `PeriodicTimerState::Armed` only after `execute()` has returned `true`.
 * This is different to the behavior of GenericTimeout, where calls to
 * `GenericTimeout::execute()` have no impact on class state.
 *
 * The `remaining()` time until period expiration is signed positive before,
 * and negative after period expiration until `execute()` is called.
 * If the timer is stopped, `remaining()` returns zero.
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
 * @ingroup	software_timer
 */
template< class Clock, typename TimestampType = modm::Timestamp >
class GenericPeriodicTimer
{
public:
	/// Create and start the timer
	GenericPeriodicTimer(const TimestampType period);

	/// Restart the timer with the current period.
	inline void
	restart();

	/// Restart the timer with a new period value.
	inline void
	restart(const TimestampType period);

	/// Stops the timer and sets isStopped() to `true`, and isExpired() to `false`.
	inline void
	stop();


	/// @return `true` exactly once during each period
	bool
	execute();


	/// @return the time until (positive time) or since (negative time) expiration, or 0 if stopped
	inline typename TimestampType::SignedType
	remaining() const;


	/// @return the current state of the timer
	PeriodicTimerState
	getState() const;

	/// @return `true` if the timer has been stopped, `false` otherwise
	inline bool
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
 * @ingroup		software_timer
 */
using ShortPeriodicTimer = GenericPeriodicTimer< ::modm::Clock, ShortTimestamp>;

/// Periodic software timer for up to 24 days with millisecond resolution.
/// @ingroup	software_timer
using PeriodicTimer      = GenericPeriodicTimer< ::modm::Clock, Timestamp>;

}	// namespace

#include "periodic_timer_impl.hpp"

#endif // MODM_PERIODIC_TIMER_HPP
