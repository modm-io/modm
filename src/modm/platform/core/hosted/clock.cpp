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

modm::chrono::milli_clock::time_point modm_weak
modm::chrono::milli_clock::now() noexcept
{
	const auto time = std::chrono::steady_clock::now().time_since_epoch();
	return time_point{std::chrono::duration_cast<duration>(time)};
}

modm::chrono::micro_clock::time_point modm_weak
modm::chrono::micro_clock::now() noexcept
{
	const auto time = std::chrono::high_resolution_clock::now().time_since_epoch();
	return time_point{std::chrono::duration_cast<duration>(time)};
}
