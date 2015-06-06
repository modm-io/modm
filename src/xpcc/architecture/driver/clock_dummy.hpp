// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_CLOCK_DUMMY_HPP
#define XPCC_CLOCK_DUMMY_HPP

#include <xpcc/processing/timer/timestamp.hpp>

namespace xpcc
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

}	// namespace xpcc

#endif // XPCC_CLOCK_DUMMY_HPP
