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
 * Software timer
 *
 * Extra care must be taken when not calling the isExpired() method
 * for more than ~30000 ticks (which corresponds to about 30s with the
 * default millisecond resolution). Due to an overflow in the implementation
 * this might add an additional delay of up to ~30000 ticks in the worst
 * case.
 * Just call restart() without any parameter before reusing the timer
 * to avoid this behaviour.
 *
 * This won't happen during normal operation.
 *
 * @warning	Never use this timer when a precise timebase is needed!
 *
 * @tparam	T	Used timer, default is xpcc::Clock() which should have
 * 				a millisecond resolution.
 *
 * @see		Timeout
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	processing
 */
template< class Clock, typename TimestampType >
class PeriodicTimerBase
{
public:
	/**
	 * Create and start the timer.
	 *
	 * @param interval
	 * 			Timed interval in millisecond resolution (depends on
	 * 			xpcc::Clock). Set to zero to create a stopped timer.
	 */
	PeriodicTimerBase(const TimestampType interval = 0);

	/// Stop the timer
	inline void
	stop();

	/// Check if the timer is running
	inline bool
	isRunning() const;

	/// Restart the current interval.
	inline void
	restart();

	/// Set a new interval
	inline void
	restart(const TimestampType interval);

	/**
	 * Check if a new period has started
	 *
	 * This function can be used to easily write sections that are
	 * executed at defined periods.
	 *
	 * @code
	 * xpcc::PeriodicTimer<> timer(50);
	 * while (1)
	 * {
	 *     if (timer.isExpired()) {
	 *         // will be executed approximately every 50 ms
	 *     }
	 * }
	 * @endcode
	 *
	 * @return	`true` if entering a new period, `false` otherwise
	 */
	inline bool
	isExpired();

	/// Returns the remaining time until a timeout.
	inline TimestampType
	remaining();

private:
	TimestampType interval;
	TimeoutBase<Clock, TimestampType> timeout;
};

template< class Clock = ::xpcc::Clock >
using PeriodicTimer = PeriodicTimerBase<Clock, Timestamp>;

template< class Clock = ::xpcc::Clock >
using LongPeriodicTimer = PeriodicTimerBase<Clock, LongTimestamp>;

}	// namespace

#include "periodic_timer_impl.hpp"

#endif // XPCC_PERIODIC_TIMER_HPP
