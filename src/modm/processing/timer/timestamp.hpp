/*
 * Copyright (c) 2019, Raphael Lehmann
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
#include <modm/architecture/interface/clock.hpp>

namespace modm
{
/// @ingroup    modm_processing_timer
/// @{

/// 16bit duration, which can hold up to 65 seconds at millisecond resolution.
using        ShortDuration = std::chrono::duration<uint16_t, std::milli>;
/// 32bit duration, which can hold up to 49 days at millisecond resolution.
using             Duration = std::chrono::duration<uint32_t, std::milli>;
/// 16bit precise duration, which can hold up to 65 milliseconds at microsecond resolution.
using ShortPreciseDuration = std::chrono::duration<uint16_t, std::micro>;
/// 32bit precise duration, which can hold up to 71 minutes at microsecond resolution.
using      PreciseDuration = std::chrono::duration<uint32_t, std::micro>;


/// 16bit time point, which can hold up to 65 seconds at millisecond resolution.
using        ShortTimestamp = std::chrono::time_point<modm::chrono::milli_clock, ShortDuration>;
/// 32bit time point, which can hold up to 49 days at millisecond resolution.
using             Timestamp = modm::chrono::milli_clock::time_point;
/// 16bit precise time point, which can hold up to 65 milliseconds at microsecond resolution.
using ShortPreciseTimestamp = std::chrono::time_point<modm::chrono::micro_clock, ShortPreciseDuration>;
/// 32bit precise time point, which can hold up to 71 minutes at microsecond resolution.
using      PreciseTimestamp = modm::chrono::micro_clock::time_point;

/// @}
}	// namespace modm
