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

template< class Clock, class TimestampType >
xpcc::PeriodicTimerState
xpcc::GenericPeriodicTimer<Clock, TimestampType>::getState()
{
	return PeriodicTimerState(timeout.getState());
}

template< class Clock , typename TimestampType >
bool
xpcc::GenericPeriodicTimer<Clock, TimestampType>::isStopped() const
{
	return timeout.isStopped();
}

template< class Clock , typename TimestampType >
bool
xpcc::GenericPeriodicTimer<Clock, TimestampType>::execute()
{
	if (timeout.execute())
	{
		TimestampType now = Clock::template now<TimestampType>();

		do
		{
			timeout.endTime = timeout.endTime + period;
		}
		while(timeout.endTime <= now);

		timeout.state = timeout.ARMED;
		return true;
	}
	return false;
}

template< class Clock, class TimestampType >
typename TimestampType::Type
xpcc::GenericPeriodicTimer<Clock, TimestampType>::remaining() const
{
	return timeout.remaining();
}

