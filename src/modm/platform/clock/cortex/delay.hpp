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

#ifndef	MODM_PLATFORM_DELAY_HPP
#define	MODM_PLATFORM_DELAY_HPP

#include <stdint.h>
#include <modm/architecture/utils.hpp>

extern "C" void _delay_ns(uint32_t ns);
extern "C" void _delay_us(uint32_t us);
extern "C" void _delay_ms(uint32_t ms);

namespace modm
{

/// @cond
modm_always_inline void
delayNanoseconds(uint16_t ns)
{
	::_delay_ns(ns);
}

modm_always_inline void
delayMicroseconds(uint16_t us)
{
	::_delay_us(us);
}

modm_always_inline void
delayMilliseconds(uint16_t ms)
{
	::_delay_ms(ms);
}
/// @endcond

}

#endif	// MODM_PLATFORM_DELAY_HPP
