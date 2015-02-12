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
template< class Clock, class TimestampType = xpcc::Timestamp >
class GenericTimeout
{
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


	/// @return `true` exactly once, after the timeout expired
	bool
	execute();


	/// @return the time until (positive time) or since (negative time) expiration, or 0 if stopped
	inline typename TimestampType::SignedType
	remaining() const;


	/// @return the current state of the timeout
	TimeoutState
	getState();

	/// @return `true` if the timeout is stopped, `false` otherwise
	inline bool
	isStopped() const;

	/// @return `true` if the timeout has expired, `false` otherwise
	inline bool
	isExpired();

	/// @return `true` if the timeout is armed (not stopped and not expired), `false` otherwise
	inline bool
	isArmed();

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
	uint8_t state;

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
 * @ingroup	processing
 */
using ShortTimeout = GenericTimeout<::xpcc::Clock, ShortTimestamp>;

/// Software timeout for up to 24 days with millisecond resolution.
/// @ingroup	processing
using Timeout      = GenericTimeout<::xpcc::Clock, Timestamp>;

}	// namespace xpcc

#include "timeout_impl.hpp"

#endif // XPCC_TIMEOUT_HPP
