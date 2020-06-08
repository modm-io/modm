/*
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2009, 2011, Martin Rosekeit
 * Copyright (c) 2012, Sascha Schade
 * Copyright (c) 2012, 2015, 2020, Niklas Hauser
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

template< class Clock, class Duration >
template< typename Rep, typename Period >
modm::GenericTimeout<Clock, Duration>::GenericTimeout(std::chrono::duration<Rep, Period> interval)
{
	restart(interval);
}

template< class Clock, class Duration >
void
modm::GenericTimeout<Clock, Duration>::restart()
{
	restart(_interval);
}

template< class Clock, class Duration >
template< typename Rep, typename Period >
void
modm::GenericTimeout<Clock, Duration>::restart(std::chrono::duration<Rep, Period> interval)
{
	if (0 <= interval.count())
	{
		const auto cast_interval = std::chrono::duration_cast<duration>(interval);
		modm_assert_continue_fail_debug(interval.count() <= duration::max().count(), "tmr.size",
				"Timer interval must be smaller than the maximal duration!", interval.count());

		_start = now();
		_interval = cast_interval;
		_state = ARMED;
	}
	else {
		modm_assert_continue_fail_debug(false, "tmr.neg",
			"Timer interval must be larger than zero!", interval.count());
		stop();
	}
}

template< class Clock, class Duration >
void
modm::GenericTimeout<Clock, Duration>::stop()
{
	_state = STOPPED;
	_interval = duration{0};
}

// ----------------------------------------------------------------------------
template< class Clock, class Duration >
bool
modm::GenericTimeout<Clock, Duration>::execute()
{
	if (_state & (EXECUTED | STOPPED))
		return false;

	if ((_state == EXPIRED) or checkExpiration())
	{
		_state = (EXPIRED | EXECUTED);
		return true;
	}

	// can only be fired once!
	return false;
}

template< class Clock, class Duration >
typename modm::GenericTimeout<Clock, Duration>::wide_signed_duration
modm::GenericTimeout<Clock, Duration>::remaining() const
{
	if (_state == STOPPED)
		return wide_signed_duration{0};
	return  std::chrono::duration_cast<wide_signed_duration>(_interval) +
			std::chrono::time_point_cast<wide_signed_duration>(_start) -
			std::chrono::time_point_cast<wide_signed_duration>(now());
}

template< class Clock, class Duration >
typename modm::GenericTimeout<Clock, Duration>::duration
modm::GenericTimeout<Clock, Duration>::interval() const
{
	return _interval;
}

// ----------------------------------------------------------------------------
template< class Clock, class Duration >
modm::TimerState
modm::GenericTimeout<Clock, Duration>::state() const
{
	if (checkExpiration())
		_state = (_state & ~ARMED) | EXPIRED;
	return TimerState(_state & STATUS_MASK);
}

template< class Clock, class Duration >
bool
modm::GenericTimeout<Clock, Duration>::isStopped() const
{
	// we do not need to use `getState()` here, since stopping a timeout
	// has to be done by the user, it will not stop itself.
	return _state == STOPPED;
}

template< class Clock, class Duration >
bool
modm::GenericTimeout<Clock, Duration>::isArmed() const
{
	return state() == TimerState::Armed;
}

template< class Clock, class Duration >
bool
modm::GenericTimeout<Clock, Duration>::isExpired() const
{
	return state() == TimerState::Expired;
}

// ----------------------------------------------------------------------------
template< class Clock, class Duration >
bool
modm::GenericTimeout<Clock, Duration>::checkExpiration() const
{
	return (_state & ARMED) and (now() - _start) >= _interval;
}

template< class Clock, class Duration >
typename modm::GenericTimeout<Clock, Duration>::time_point
modm::GenericTimeout<Clock, Duration>::now() const
{
	return std::chrono::time_point_cast<duration>(Clock::now());
}
