// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_TIMEOUT_HPP
#define XPCC_TIMEOUT_HPP

#include <xpcc/architecture/driver/clock.hpp>

#include "timestamp.hpp"

namespace xpcc
{

// forward declaration for friending
template< class Clock, class TimestampType >
class PeriodicTimerBase;

/**
 * Software timer
 *
 * Has to be polled to check if it has expired.
 *
 * Extra care must be taken when not calling the isExpired() method
 * for more than ~30000 ticks (which corresponds to about 30s with the
 * default millisecond resolution) when the timeout is active. Due to an
 * overflow in the implementation this might add an additional delay of
 * up to ~30000 ticks in the worst case.
 *
 * Call isExpired() at least once every 30000 ticks before the timeout
 * value is reached to avoid this behaviour.
 *
 * @see		PeriodicTimer
 * @tparam	T	Used timer, default is xpcc::Clock() which has
 * 				a millisecond resolution.
 *
 * Usage:
 * @include	timeout.cpp
 *
 * Complete example for the ATMega644:
 * @include timeout/main.cpp
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	processing
 */
template< class Clock, class TimestampType >
class TimeoutBase
{
	friend class
	PeriodicTimerBase<Clock, TimestampType>;

public:
	TimeoutBase();

	/// Create and start the timeout
	TimeoutBase(const TimestampType time);

	/**
	 * Check if the given time has passed.
	 *
	 * If isExpired() changes to `true`, it will keep this value until
	 * a call of restart().
	 */
	bool
	isExpired();

	/**
	 * Check if the timer was started and was not stopped.
	 *
	 * If isActive() changes to `false`, it will keep this value until
	 * a call of restart().
	 */
	inline bool
	isActive() const;

	/**
	 * Stop the timer
	 *
	 * Sets isExpired() to `true`, and isActive() to `false`.
	 */
	inline void
	stop();

	/// Set a new timeout value.
	void
	restart(TimestampType time);

	/// Returns the remaining time until a timeout.
	inline TimestampType
	remaining();

private:
	enum State
	{
		STOPPED = 0,
		EXPIRED = 2,
		ACTIVE = 4,
		RUNNING = 6,	// used for PeriodicTimer
	};

	TimestampType endTime;
	State state;
};

template< class Clock = ::xpcc::Clock >
using Timeout = TimeoutBase<Clock, Timestamp>;

template< class Clock = ::xpcc::Clock >
using LongTimeout = TimeoutBase<Clock, LongTimestamp>;

}	// namespace xpcc

#include "timeout_impl.hpp"

#endif // XPCC_TIMEOUT_HPP
