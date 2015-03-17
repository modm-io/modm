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
	endTime(0), state(STOPPED)
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
	state = ARMED;
}

template< class Clock, class TimestampType >
void
xpcc::GenericTimeout<Clock, TimestampType>::stop()
{
	state = STOPPED;
}

// ----------------------------------------------------------------------------
template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::execute()
{
	if (state & EXECUTED)
		return false;

	if (state == EXPIRED || checkExpiration())
	{
		state = EXPIRED | EXECUTED;
		return true;
	}

	// can only be fired once!
	return false;
}

template< class Clock, class TimestampType >
typename TimestampType::SignedType
xpcc::GenericTimeout<Clock, TimestampType>::remaining() const
{
	if (state != STOPPED)
		return (endTime - Clock::template now<TimestampType>()).getTime();

	return 0;
}

// ----------------------------------------------------------------------------
template< class Clock, class TimestampType >
xpcc::TimeoutState
xpcc::GenericTimeout<Clock, TimestampType>::getState() const
{
	if (checkExpiration())
	{
		state &= ~ARMED;
		state |= EXPIRED;
	}
	return TimeoutState(state & STATUS_MASK);
}

template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::isStopped() const
{
	// we do not need to use `getState()` here, since stopping a timeout
	// has to be done by the user, it will not stop itself.
	return state == STOPPED;
}

template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::isArmed() const
{
	return getState() == TimeoutState::Armed;
}

template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::isExpired() const
{
	return getState() == TimeoutState::Expired;
}

// ----------------------------------------------------------------------------
template< class Clock, class TimestampType >
bool
xpcc::GenericTimeout<Clock, TimestampType>::checkExpiration() const
{
	return (state & ARMED) and (Clock::template now<TimestampType>() >= endTime);
}
