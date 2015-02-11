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
xpcc::GenericTimeout<Clock, TimestampType>::GenericTimeout() :
	endTime(0), state(0)
{
}

template< class Clock, class TimestampType >
xpcc::GenericTimeout<Clock, TimestampType>::GenericTimeout(const TimestampType time)
{
	restart(time);
}

template< class Clock, class TimestampType >
void
xpcc::GenericTimeout<Clock, TimestampType>::restart(TimestampType time)
{
	endTime = Clock::template now<TimestampType>() + time;
	// clear FIRED flag
	state = RUNNING;
}

template< class Clock, class TimestampType >
void
xpcc::GenericTimeout<Clock, TimestampType>::stop()
{
	state &= ~(EXPIRED | RUNNING);
}

// ----------------------------------------------------------------------------
template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::isStopped() const
{
	return !(state & (EXPIRED | RUNNING));
}

template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::isExpired()
{
	if (checkExpiration())
	{
		state = EXPIRED | FIRED;
		return true;
	}

	// can only be fired once!
	return false;
}

template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::hasFired()
{
	if (checkExpiration())
	{
		state &= ~FIRED;
		return false;
	}
	return (state & FIRED);
}


template< class Clock, class TimestampType >
TimestampType
xpcc::GenericTimeout<Clock, TimestampType>::remaining() const
{
	if (state >= RUNNING)
		return (endTime - Clock::template now<TimestampType>());

	return 0;
}

// ----------------------------------------------------------------------------
template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::checkExpiration() const
{
	return (state >= RUNNING) and (Clock::template now<TimestampType>() >= endTime);
}
