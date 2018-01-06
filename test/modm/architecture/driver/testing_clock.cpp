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

#include <modm/architecture/interface/clock.hpp>

// This class is only generated on hosted targets!
modm::Clock::Type modm::Clock::time = 0;

template< typename TimestampType >
TimestampType
modm::Clock::now()
{
	return TimestampType(time);
}

void
modm::Clock::increment(uint_fast16_t /* step */)
{
	/* tumbleweed */
}

// explicit declaration of what member function templates we need to generate
template modm::ShortTimestamp modm::Clock::now();
template modm::Timestamp modm::Clock::now();

