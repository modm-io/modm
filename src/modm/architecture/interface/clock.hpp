/*
 * Copyright (c) 2009-2010, Martin Rosekeit
 * Copyright (c) 2009-2011, 2013, 2015, Fabian Greif
 * Copyright (c) 2011-2018, Niklas Hauser
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2015-2016, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_INTERFACE_CLOCK_HPP
#define	MODM_INTERFACE_CLOCK_HPP

#include <modm/architecture/utils.hpp>
#include <modm/processing/timer/timestamp.hpp>

namespace modm
{

/**
 * Interface to the system wide tick timer.
 *
 * This class maintains a 32-bit counter which is incremented every 1ms.
 *
 * Example:
 * @code
 * // Interrupt every 1ms
 * ISR(TIMER)
 * {
 *     modm::Clock::increment();
 * }
 * @endcode
 * @ingroup modm_architecture_clock
 */
class Clock
{
public:
	using Type = uint32_t;

public:
	/// Get the current time, either as `Timestamp` or `LongTimestamp`.
	/// Provides an atomic access to the current time
	template< typename TimestampType = Timestamp >
	static TimestampType
	now();

	static inline ShortTimestamp
	nowShort()
	{
		return now<ShortTimestamp>();
	}

public:
	/// Update the current time
	static void
	increment(uint_fast16_t step = 1);

protected:
	static Type time;
};

}	// namespace modm

#endif	// MODM_INTERFACE_CLOCK_HPP
