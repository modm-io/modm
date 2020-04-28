/*
 * Copyright (c) 2009-2011, 2015, Fabian Greif
 * Copyright (c) 2010-2011, Georgi Grinshpun
 * Copyright (c) 2012, 2015, 2017, 2020, Niklas Hauser
 * Copyright (c) 2013-2014, Kevin Läufer
 * Copyright (c) 2017, Sascha Schade
 * Copyright (c) 2019, Raphael Lehmann
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once
#include "timeout.hpp"

namespace modm
{

/**
 * Generic periodic software timeout class for variable timebase and timestamp width.
 *
 * @see		GenericTimeout
 *
 * @tparam	Clock
 * 		Used clock which inherits from modm::Clock, may have a variable timebase.
 * @tparam	Duration
 * 		Used timestamp which is compatible with the chosen Clock.
 *
 * @author	Fabian Greif
 * @author	Niklas Hauser
 * @ingroup	modm_processing_timer
 */
template< class Clock, class Duration  >
class GenericPeriodicTimer : public GenericTimeout<Clock, Duration>
{
public:
	// Inherit all constructors
	using GenericTimeout<Clock, Duration>::GenericTimeout;

	/**
	 * For a duration of 0, this function will always expire, but only return 1.
	 *
	 * @return the number of missed periods, or zero if not expired yet
	 */
	size_t
	execute()
	{
		if (GenericTimeout<Clock, Duration>::execute())
		{
			size_t count{0};
			if (this->_interval.count())
			{
				const auto now = this->now();
				while(1)
				{
					this->_start += this->_interval;
					const auto diff{now - this->_start};
					if (diff != this->_interval) count++;
					if (diff < this->_interval) break;
				}
			}
			else {
				this->_start = this->now();
				count = 1;
			}
			this->_state = this->ARMED;
			return count;
		}
		return 0;
	}
};

/**
 * Periodic software timer for up to 65 seconds with millisecond resolution.
 *
 * Extra care must be taken when not calling the `execute()` method
 * for more than 65 seconds. Due to an overflow in the implementation
 * this might add an additional delay of up to 65s ticks in the worst
 * case.
 * Always call `restart()` before reusing the timer to avoid this behaviour.
 *
 * If you need a longer time period, use PeriodicTimer.
 *
 * @ingroup		modm_processing_timer
 */
using        ShortPeriodicTimer = GenericPeriodicTimer< Clock, ShortDuration >;

/// Periodic software timer for up to 49 days with millisecond resolution.
/// @ingroup	modm_processing_timer
using             PeriodicTimer = GenericPeriodicTimer< Clock, Duration >;

/// Periodic software timer for up to 65 milliseconds with microsecond resolution.
/// @ingroup	modm_processing_timer
using ShortPrecisePeriodicTimer = GenericPeriodicTimer< PreciseClock, ShortPreciseDuration >;

/// Periodic software timer for up to 71 minutes with microsecond resolution.
/// @ingroup	modm_processing_timer
using      PrecisePeriodicTimer = GenericPeriodicTimer< PreciseClock, PreciseDuration >;

/// @cond
using PeriodicTimerState [[deprecated("Use `modm::TimerState` instead!")]] = TimerState;
/// @endcond

}	// namespace
