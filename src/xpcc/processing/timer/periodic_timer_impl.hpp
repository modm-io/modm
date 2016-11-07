/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2010-2011, Georgi Grinshpun
 * Copyright (c) 2012, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
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
xpcc::GenericPeriodicTimer<Clock, TimestampType>::getState() const
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
typename TimestampType::SignedType
xpcc::GenericPeriodicTimer<Clock, TimestampType>::remaining() const
{
	return timeout.remaining();
}

