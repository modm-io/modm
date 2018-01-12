/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012, 2015, 2017-2018, Niklas Hauser
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

#include <modm/architecture/interface/clock.hpp>

/**
 * Gain full access to modm::Clock::time.
 */
class TestingClock : public modm::Clock
{
public:
	// expose protected members
	using modm::Clock::time;
};

#endif
