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
xpcc::PeriodicTimerBase<Clock, TimestampType>::PeriodicTimerBase(const TimestampType interval) :
	interval(interval), timeout(interval)
{
}

template< class Clock , typename TimestampType >
void
xpcc::PeriodicTimerBase<Clock, TimestampType>::stop()
{
	timeout.stop();
}

template< class Clock , typename TimestampType >
bool
xpcc::PeriodicTimerBase<Clock, TimestampType>::isRunning() const
{
	return timeout.isRunning();
}

template< class Clock , typename TimestampType >
void
xpcc::PeriodicTimerBase<Clock, TimestampType>::restart(const TimestampType interval)
{
	this->interval = interval;
	restart();
}

template< class Clock , typename TimestampType >
void
xpcc::PeriodicTimerBase<Clock, TimestampType>::restart()
{
	timeout.restart(interval);
}

template< class Clock , typename TimestampType >
bool
xpcc::PeriodicTimerBase<Clock, TimestampType>::isExpired()
{
	if (timeout.isExpired())
	{
		timeout.restart(interval);
		return true;
	}
	return false;
}

template< class Clock, class TimestampType >
TimestampType
xpcc::PeriodicTimerBase<Clock, TimestampType>::remaining()
{
	return timeout.remaining();
}

