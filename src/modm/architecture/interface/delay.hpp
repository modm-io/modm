/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2010, Georgi Grinshpun
 * Copyright (c) 2012, 2014, Sascha Schade
 * Copyright (c) 2012, 2014-2016, Niklas Hauser
 * Copyright (c) 2014, Kevin Läufer
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

/**
 * @ingroup		interface
 * @defgroup	delay	Blocking delays
 *
 * These functions allow you to spin for a short time using only the CPU.
 *
 * You should always prefer Software Timers (@see software_timer) over these
 * *blocking* delay functions. However, when `xpcc::Clock` is not set up yet, or
 * when you need very small delays (for example to bit-bang a protocol), you need
 * to use these delay functions.
 *
 * The only guarantee given to you is to delay for _at least_ the specified time.
 * Note that invocation of interrupts during spinning may add delay too.
 *
 * @warning
 *    You must not rely on delay functions for ANY time-keeping!
 *
 * Delay functions work at any CPU clock speed, even if changed dynamically and
 * are available very early in the startup process at hardware-init time.
 *
 * @warning
 *    Correct behavior is not guaranteed for delays over 1000ns, us or ms!
 *    Make sure to use the largest time unit possible.
 *
 * @author	Niklas Hauser
 */

#ifdef __DOXYGEN__

namespace modm
{

/// Spin for nanoseconds.
/// This function is implemented as best-effort and it's resolution will be very
/// coarse especially on platforms with very slow clocks.
/// @ingroup	delay
void
delayNanoseconds(uint16_t ns);

/// Spin for microseconds.
/// @ingroup	delay
void
delayMicroseconds(uint16_t us);

/// Spin for milliseconds.
/// @ingroup	delay
void
delayMilliseconds(uint16_t ms);

}

#else

// These functions may be implemented as header files, so we cannot rely on
// there being a link-able function and delegate this choice to the platform.
#include <modm/platform/clock/delay.hpp>

#endif

#endif	// MODM_INTERFACE_DELAY_HPP
