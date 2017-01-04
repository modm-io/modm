/*
 * Copyright (c) 2009, Martin Rosekeit
 * Copyright (c) 2009-2011, Fabian Greif
 * Copyright (c) 2012-2014, 2016, Niklas Hauser
 * Copyright (c) 2013, Sascha Schade
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#ifndef XPCC_ATXMEGA_UTILS_HPP
#define XPCC_ATXMEGA_UTILS_HPP

#include <stddef.h>		// for offsetof()
#include <modm/architecture/utils.hpp>
#include "../../../device.hpp"

namespace xpcc
{

namespace xmega
{

/**
 * CCP write helper function.
 *
 * This function is written in assembly because of the time critical
 * operation of writing to the registers.
 *
 * @param	address
 * 		A pointer to the address to write to.
 * @param	value
 * 		The value to put in to the register.
 *
 * @ingroup	xmega
 */
xpcc_always_inline static void
changeProtectedRegister(volatile uint8_t *address, uint8_t value)
{
#ifdef RAMPZ
	RAMPZ = 0;
#endif

	asm volatile (
		"movw r30, %0"		"\n\t"
		"ldi  r16, 0xD8"	"\n\t"
		"out  0x34, r16"	"\n\t"
		"st   Z,  %1"
			:
			: "r" (address), "r" (value)
			: "r16", "r30", "r31"
	);
}

/**
 * Read a calibration byte.
 *
 * Example:
 * @code
 * ADCA.CALL = readCalibrationByte(offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0));
 * @endcode
 *
 * @ingroup	xmega
 */
uint8_t
readCalibrationByte(uint8_t index);

/**
 * Reset the Xmega via software.
 *
 * @ingroup	xmega
 */
xpcc_always_inline static void
softwareReset()
{
	changeProtectedRegister(&RST_CTRL, RST_SWRST_bm);
}

}	// namespace xmega

}	// namespace xpcc

#endif	// XPCC_ATXMEGA__UTILS_HPP
