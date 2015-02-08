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
	timeout(interval), interval(interval), isRunning_(true)
{
	if (interval == 0) {
		stop();
	}
}

template< class Clock , typename TimestampType >
void
xpcc::PeriodicTimerBase<Clock, TimestampType>::stop()
{
	timeout.stop();
	isRunning_ = false;
}

template< class Clock , typename TimestampType >
bool
xpcc::PeriodicTimerBase<Clock, TimestampType>::isRunning() const
{
	return isRunning_;
}

template< class Clock , typename TimestampType >
void
xpcc::PeriodicTimerBase<Clock, TimestampType>::restart(const TimestampType interval)
{
	timeout.restart(interval);
	this->interval = interval;
	this->isRunning_ = true;
}

template< class Clock , typename TimestampType >
void
xpcc::PeriodicTimerBase<Clock, TimestampType>::restart()
{
	timeout.restart(this->interval);
	this->isRunning_ = true;
}

template< class Clock , typename TimestampType >
bool
xpcc::PeriodicTimerBase<Clock, TimestampType>::isExpired()
{
	if (isRunning_ and timeout.isExpired()) {
		timeout.restart(interval);
		return true;
	}
	else {
		return false;
	}
}
