// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_TIMEOUT_HPP
#	error	"Don't include this file directly, use 'timeout.hpp' instead!"
#endif

template< class Clock, class TimestampType >
xpcc::TimeoutBase<Clock, TimestampType>::TimeoutBase() :
	endTime(0), state(EXPIRED)
{
}

template< class Clock, class TimestampType >
xpcc::TimeoutBase<Clock, TimestampType>::TimeoutBase(const TimestampType time) :
	endTime(Clock::template now<TimestampType>() + time), state(ACTIVE)
{
}

template< class Clock, class TimestampType >
bool
xpcc::TimeoutBase<Clock, TimestampType>::isExpired()
{
	if (isActive())
	{
		if (Clock::template now<TimestampType>() >= endTime)
		{
			state = EXPIRED;
		}
		else {
			return false;
		}
	}

	return true;
}

template< class Clock, class TimestampType >
bool
xpcc::TimeoutBase<Clock, TimestampType>::isActive() const
{
	return (state >= ACTIVE);
}

template< class Clock, class TimestampType >
void
xpcc::TimeoutBase<Clock, TimestampType>::stop()
{
	state = STOPPED;
}

template< class Clock, class TimestampType >
void
xpcc::TimeoutBase<Clock, TimestampType>::restart(TimestampType time)
{
	endTime = Clock::template now<TimestampType>() + time;
	state = ACTIVE;
}

template< class Clock, class TimestampType >
TimestampType
xpcc::TimeoutBase<Clock, TimestampType>::remaining()
{
	if (isExpired())
		return 0;
	else
		return (endTime - Clock::template now<TimestampType>());
}
