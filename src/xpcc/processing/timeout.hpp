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
class GenericPeriodicTimer;

/**
 * Generic software timeout class for variable timebase and timestamp width.
 *
 * @warning	Never use this timer when a precise timebase is needed!
 *
 * @see		GenericPeriodicTimer
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
template< class Clock, class TimestampType >
class GenericTimeout
{
	friend class
	GenericPeriodicTimer<Clock, TimestampType>;

public:
	GenericTimeout();

	/// Create and start the timeout
	GenericTimeout(const TimestampType time);

	/// Set a new timeout value.
	void
	restart(TimestampType time);

	/// Stops the timer and sets isStopped() to `true`, and isExpired() to `false`.
	inline void
	stop();

	/// @return `true` if the timer has been stopped, `false` otherwise
	inline bool
	isStopped() const;

	/// Returns `true` **once**, when the timeout has expired.
	/// Calling this function sets hasFired() to `true`.
	bool
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
	inline bool
	checkExpiration() const;

private:
	TimestampType endTime;

	enum
	State : uint8_t
	{
		FIRED   = 0b001,
		EXPIRED = 0b010, // only internal
		RUNNING = 0b100,
		// STOPPED is not (EXPIRED or RUNNING) = not EXPIRED and not RUNNING
	};
	uint8_t state;
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
 * @ingroup	processing
 */
using ShortTimeout = GenericTimeout<::xpcc::Clock, ShortTimestamp>;

/// Software timeout for up to 24 days with millisecond resolution.
/// @ingroup	processing
using Timeout      = GenericTimeout<::xpcc::Clock, Timestamp>;

}	// namespace xpcc

#include "timeout_impl.hpp"

#endif // XPCC_TIMEOUT_HPP
