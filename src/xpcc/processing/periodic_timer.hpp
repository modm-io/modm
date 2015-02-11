// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_PERIODIC_TIMER_HPP
#define XPCC_PERIODIC_TIMER_HPP

#include "timeout.hpp"

namespace xpcc
{

/**
 * Generic software timeout class for variable timebase and timestamp width.
 *
 * @warning	Never use this timer when a precise timebase is needed!
 *
 * @see		GenericTimeout
 *
 * @tparam	Clock
 * 		Used clock which inherits from xpcc::Clock, may have a variable timebase.
 * @tparam	TimestampType
 * 		Used timestamp which is compatible with the chosen Clock.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	processing
 */
template< class Clock, typename TimestampType >
class GenericPeriodicTimer
{
public:
	/// Create and start the timer
	GenericPeriodicTimer(const TimestampType period);

	/// Restart the timer with the current interval.
	inline void
	restart();

	/// Restart the timer with a new interval value.
	inline void
	restart(const TimestampType interval);

	/// Stops the timer and sets isStopped() to `true`, and isExpired() to `false`.
	inline void
	stop();

	/// @return `true` if the timer has been stopped, `false` otherwise
	inline bool
	isStopped() const;

	/**
	 * Returns `true` **once**, when the period has expired.
	 * Calling this function sets hasFired() to `true`.
	 */
	inline bool
	isExpired();

	/**
	 * Check if the timer has been fired since it expired
	 * (i.e. has isExpired() been called since expiration).
	 * The state will be kept even when the timer is stopped,
	 * however, a call to restart(time) will reset this.
	 */
	bool
	hasFired();

	/// @return the time until expiration, or 0 if stopped
	inline TimestampType
	remaining() const;

//	/// @return the time until (negative time) or since (positive time) expiration, or 0 if stopped
//	inline TimeDelta
//	remaining() const;

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
 * @ingroup	processing
 */
using ShortPeriodicTimer = GenericPeriodicTimer<::xpcc::Clock, ShortTimestamp>;

/// Periodic software timer for up to 24 days with millisecond resolution.
/// @ingroup	processing
using PeriodicTimer      = GenericPeriodicTimer<::xpcc::Clock, Timestamp>;

}	// namespace

#include "periodic_timer_impl.hpp"

#endif // XPCC_PERIODIC_TIMER_HPP
