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

#ifndef	MODM_INTERFACE_DELAY_HPP
#define	MODM_INTERFACE_DELAY_HPP

#include <stdint.h>


#ifdef __DOXYGEN__

namespace modm
{

/// Spin for nanoseconds.
/// This function is implemented as best-effort and its resolution will be very
/// coarse especially on platforms with very slow clocks.
/// @ingroup modm_architecture_delay
void
delayNanoseconds(uint16_t ns);

/// Spin for microseconds.
/// @ingroup modm_architecture_delay
void
delayMicroseconds(uint16_t us);

/// Spin for milliseconds.
/// @ingroup modm_architecture_delay
void
delayMilliseconds(uint16_t ms);

}

#else

// These functions may be implemented as header files, so we cannot rely on
// there being a link-able function and delegate this choice to the platform.
#include <modm/platform/core/delay.hpp>

#endif

#endif	// MODM_INTERFACE_DELAY_HPP
