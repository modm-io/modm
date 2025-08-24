/*
 * Copyright (c) 2020, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include <modm/architecture/interface/clock.hpp>

// Allow the ability to override the clock now functions
#ifndef MODM_CHRONO_MILLI_CLOCK_NOW
#	define MODM_CHRONO_MILLI_CLOCK_NOW modm::chrono::milli_clock::now
#else
#endif
#ifndef MODM_CHRONO_MICRO_CLOCK_NOW
#	define MODM_CHRONO_MICRO_CLOCK_NOW modm::chrono::micro_clock::now
#endif

modm::chrono::milli_clock::time_point modm_weak
MODM_CHRONO_MILLI_CLOCK_NOW() noexcept
{
	const auto time = std::chrono::steady_clock::now().time_since_epoch();
	return modm::chrono::milli_clock::time_point{
		std::chrono::duration_cast<modm::chrono::milli_clock::duration>(time)};
}

modm::chrono::micro_clock::time_point modm_weak
MODM_CHRONO_MICRO_CLOCK_NOW() noexcept
{
	const auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
	return modm::chrono::micro_clock::time_point{
		std::chrono::duration_cast<modm::chrono::micro_clock::duration>(time)};
}
