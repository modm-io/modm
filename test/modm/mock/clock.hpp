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

#pragma once
#include <stdint.h>
#include <modm/architecture/interface/clock.hpp>

/// @ingroup modm_test_mock_clock
namespace modm_test::chrono
{

class milli_clock : modm::chrono::milli_clock
{
public:
	static inline void setTime(std::chrono::milliseconds time)
	{ setTime(time.count()); }
	static void setTime(uint32_t milliseconds);

	static inline void increment(std::chrono::milliseconds time)
	{ increment(time.count()); }
	static void increment(uint32_t milliseconds);
};

class micro_clock : modm::chrono::micro_clock
{
public:
	static inline void setTime(std::chrono::microseconds time)
	{ setTime(time.count()); }
	static void setTime(uint32_t microseconds);

	static inline void increment(std::chrono::microseconds time)
	{ increment(time.count()); }
	static void increment(uint32_t microseconds);
};

} // namespace modm_test
