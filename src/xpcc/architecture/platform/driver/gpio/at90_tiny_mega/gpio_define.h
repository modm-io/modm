// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2013, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#ifndef	XPCC_AT90_ATTINY_ATMEGA_GPIO_DEFINES_H
#define	XPCC_AT90_ATTINY_ATMEGA_GPIO_DEFINES_H

// Atmel made a mess with the register names for GPIO between the "old" devices.
// To avoid to much conditional compiling we try to mimic the new names for
// the old devices.

#if defined (PCMSK) && not defined (PCMSK0)
#	define PCMSK0	PCMSK
#endif
#if defined (PCIE) && not defined (PCIE0)
#	define PCIE0	PCIE
#endif
#if defined (PCIF) && not defined (PCIF0)
#	define PCIF0	PCIF
#endif

// Some more redefines because Atmel is weird
#ifndef EIMSK
#	if defined GIMSK
#		define EIMSK	GIMSK
#	elif defined GICR
#		define EIMSK	GICR
#	else
#		error "Cannot find register for setting/clearing INTx!"
#	endif
#endif

#ifndef EIFR
#	if defined GIFR
#		define EIFR		GIFR
#	else
#		error "Cannot find register for reading/acknowledging INTFx!"
#	endif
#endif

#ifndef PCICR
#	if defined EIMSK
#		define PCICR	EIMSK
#	elif defined GIMSK
#		define PCICR	GIMSK
#	elif defined GICR
#		define PCICR	GICR
#	else
#		error "Cannot find register for setting/clearing PCIEx!"
#	endif
#endif

#ifndef PCIFR
#	if defined EIFR
#		define PCIFR	EIFR
#	elif defined GIFR
#		define PCIFR	GIFR
#	else
#		error "Cannot find register for reading/acknowledging PCIFx!"
#	endif
#endif

#endif	// XPCC_AT90_ATTINY_ATMEGA_GPIO_DEFINES_H

