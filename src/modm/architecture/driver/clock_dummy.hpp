/*
 * Copyright (c) 2013, Kevin Läufer
 * Copyright (c) 2013, Sascha Schade
 * Copyright (c) 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_CLOCK_DUMMY_HPP
#define MODM_CLOCK_DUMMY_HPP

#include <modm/processing/timer/timestamp.hpp>

namespace modm
{

/**
 * Dummy implementation of a clock where the user can control the time.
 *
 * This is useful in unittests where a precise control of the system time is necessary.
 */
class ClockDummy
{
public:
	typedef uint32_t Type;

public:
	template< typename TimestampType = Timestamp >
	static TimestampType
	now()
	{
		return TimestampType(time);
	}

	static ShortTimestamp
	nowShort()
	{
		return ShortTimestamp(time);
	}

	static void
	setTime(const Type time)
	{
		ClockDummy::time = time;
	}

private:
	static Type time;
};

}	// namespace modm

#endif // MODM_CLOCK_DUMMY_HPP
