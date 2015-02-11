// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_PERIODIC_TIMER_HPP
#	error	"Don't include this file directly, use 'periodic_timeout.hpp' instead!"
#endif

template< class Clock , typename TimestampType >
xpcc::GenericPeriodicTimer<Clock, TimestampType>::GenericPeriodicTimer(const TimestampType period) :
	period(period), timeout(period)
{
}

template< class Clock , typename TimestampType >
void
xpcc::GenericPeriodicTimer<Clock, TimestampType>::restart()
{
	timeout.restart(period);
}

template< class Clock , typename TimestampType >
void
xpcc::GenericPeriodicTimer<Clock, TimestampType>::restart(const TimestampType period)
{
	this->period = period;
	restart();
}

template< class Clock , typename TimestampType >
void
xpcc::GenericPeriodicTimer<Clock, TimestampType>::stop()
{
	timeout.stop();
}

template< class Clock , typename TimestampType >
bool
xpcc::GenericPeriodicTimer<Clock, TimestampType>::isStopped() const
{
	return timeout.isStopped();
}

template< class Clock , typename TimestampType >
bool
xpcc::GenericPeriodicTimer<Clock, TimestampType>::isExpired()
{
	if (timeout.isExpired())
	{
		TimestampType now = Clock::template now<TimestampType>();

		do
		{
			timeout.endTime = timeout.endTime + period;
		}
		while(timeout.endTime <= now);

		// do **not** clear FIRED here!
		timeout.state = (timeout.state & ~timeout.EXPIRED) | timeout.RUNNING;
		return true;
	}
	return false;
}

template< class Clock , typename TimestampType >
bool
xpcc::GenericPeriodicTimer<Clock, TimestampType>::hasFired()
{
	return timeout.hasFired();
}

template< class Clock, class TimestampType >
TimestampType
xpcc::GenericPeriodicTimer<Clock, TimestampType>::remaining() const
{
	return timeout.remaining();
}

