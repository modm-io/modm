// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_ATTINY_ATMEGA__GPIO_DEFINES_H
#define	XPCC_ATTINY_ATMEGA__GPIO_DEFINES_H

// Atmel made a mess with the register names for GPIO between the "old" devices.
// To avoid to much conditional compiling we try to mimic the new names for
// the old devices.

#if defined (PCMSK)
#	define PCMSK0	PCMSK
#	define PCIE0	PCIE
#	define PCIF0	PCIF
#endif

#endif	// XPCC_ATTINY_ATMEGA__GPIO_DEFINES_H

