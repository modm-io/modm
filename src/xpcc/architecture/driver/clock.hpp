/*
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2011-2012, 2014-2015, Niklas Hauser
 * Copyright (c) 2015, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_CLOCK_HPP
#define	XPCC_CLOCK_HPP

#include <modm/architecture/utils.hpp>
#include <modm/processing/timer/timestamp.hpp>

namespace xpcc
{

/**
 * Internal system-tick timer
 *
 * This class is implemented using `gettimeofday()` from <sys/time.h> for
 * any Unix-OS.
 *
 * For Cortex-M targets the user has to enable the `xpcc::SysTick` timer.
 *
 * For the AVRs targets the user has to use the increment() method to
 * generate a suitable timebase, preferably by incrementing the time
 * value inside a timer interrupt function.
 *
 * Example:
 * @code
 * // Interrupt every 1ms
 * ISR(TIMER)
 * {
 *     xpcc::Clock::increment();
 * }
 * @endcode
 *
 * @ingroup	architecture
 */
class Clock
{
public:
	typedef uint32_t Type;

public:
	/**
	 * Get the current time, either as Timestamp or LongTimestamp.
	 *
	 * Provides an atomic access to the current time
	 */
	template< typename TimestampType = Timestamp >
	static TimestampType
	now();

	static inline ShortTimestamp
	nowShort()
	{
		return now<ShortTimestamp>();
	}

#if !defined(XPCC_OS_HOSTED)
	/// Set the current time
	static inline void
	increment(uint_fast16_t step = 1)
	{
		time += step;
	}
#endif

protected:
	static Type time;
};

}	// namespace xpcc

#endif	// XPCC_CLOCK_HPP
