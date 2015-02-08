// coding: utf-8
/* Copyright (c) 2009, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	TESTING_CLOCK_HPP
#define	TESTING_CLOCK_HPP

#include <xpcc/architecture/driver/clock.hpp>

/**
 * Gain full access to xpcc::Clock
 *
 * This class is only useful if the define XPCC__CLOCK_TESTMODE is set to 1.
 * Otherwise the `time` member will be ignored on any non AVR target.
 */
class TestingClock : public xpcc::Clock
{
public:
	// expose protected members
	using xpcc::Clock::time;
};

#endif
