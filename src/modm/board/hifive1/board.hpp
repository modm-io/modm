/*
 * Copyright (c) 2019 Benjamin Weps
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef MODM_HIFIVE1_HPP
#define MODM_HIFIVE1_HPP

#include <modm/platform.hpp>
#include <cstdint>
//#include <modm/architecture/interface/clock.hpp>
/// @ingroup modm_hifive1

namespace Board{

struct SystemClock {
	static constexpr uint32_t Frequency = 320'000'000;

	static bool inline
	enable()
	{
		return true;
	}
};

inline void
initialize()
{

}

}

#endif	// MODM_HIFIVE1_HPP
