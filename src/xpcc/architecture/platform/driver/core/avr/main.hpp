/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2010, Fabian Greif
 * Copyright (c) 2012, 2015-2016, Niklas Hauser
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef	MODM_AVR_MAIN_HPP
#define	MODM_AVR_MAIN_HPP

#include <stdint.h>

// saves a couple of bytes of stack and program.
int main(void) __attribute__((OS_main));

#endif	// MODM_AVR_MAIN_HPP
