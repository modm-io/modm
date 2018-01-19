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
#include <util/delay.h>
#include <modm/architecture/utils.hpp>

// FIXME: make this more accurate for AVRs
namespace modm
{

/// @cond
void modm_always_inline
delayNanoseconds(uint16_t /*ns*/)
{
	_delay_us(1);
}

modm_always_inline void
delayMicroseconds(uint16_t us)
{
	while(us--) _delay_us(1);
}

modm_always_inline void
delayMilliseconds(uint16_t ms)
{
	while(ms--) _delay_ms(1);
}
/// @endcond

}	// namespace modm

#endif	// MODM_PLATFORM_DELAY_HPP
