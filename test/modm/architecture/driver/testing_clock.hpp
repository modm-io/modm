/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2015, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	TESTING_CLOCK_HPP
#define	TESTING_CLOCK_HPP

#include <modm/architecture/driver/clock.hpp>

/**
 * Gain full access to modm::Clock
 *
 * This class is only useful if the define MODM_CLOCK_TESTMODE is set to 1.
 * Otherwise the `time` member will be ignored on any non AVR target.
 */
class TestingClock : public modm::Clock
{
public:
	// expose protected members
	using modm::Clock::time;
};

#endif
