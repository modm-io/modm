/*
 * Copyright (c) 2017, Fabian Greif
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_CLOCK_DUMMY_HPP
#define	MODM_CLOCK_DUMMY_HPP

#include <modm/architecture/utils.hpp>
#include <modm/processing/timer/timestamp.hpp>

namespace modm
{

/**
 * Dummy clock implementation for testing.
 *
 * @ingroup	clock
 */
class ClockDummy
{
public:
	/// Get the current time, either as `Timestamp` or `LongTimestamp`.
	/// Provides an atomic access to the current time
	template< typename TimestampType = Timestamp >
	static inline TimestampType
	now()
	{
		return TimestampType(time.getTime());
	}

	static inline ShortTimestamp
	nowShort()
	{
		return now<ShortTimestamp>();
	}

	/// Update the current time
	static inline void
	increment(uint_fast16_t step = 1)
	{
		time = time + Timestamp(step);
	}

	static inline void
	setTime(Timestamp timestamp)
	{
		time = timestamp;
	}

protected:
	static Timestamp time;
};

}	// namespace modm

#endif
