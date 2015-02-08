// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_CLOCK_HPP
#define	XPCC_CLOCK_HPP

#include <xpcc/architecture/utils.hpp>
#include <xpcc/processing/timestamp.hpp>

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

#if !defined(XPCC__OS_HOSTED)
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
