/*
 * Copyright (c) 2017-2018, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/clock.hpp>
#include <modm/architecture/interface/atomic_lock.hpp>

modm::Clock::Type modm::Clock::time;

template< typename TimestampType >
TimestampType
modm::Clock::now()
{
	typename TimestampType::Type tempTime;
	{
		atomic::Lock lock;
		tempTime = typename TimestampType::Type(time);
	}
	return TimestampType(tempTime);
}

void
modm::Clock::increment(uint_fast16_t step)
{
	time += step;
}

// explicit declaration of what member function templates we need to generate
template modm::ShortTimestamp modm::Clock::now();
template modm::Timestamp modm::Clock::now();
