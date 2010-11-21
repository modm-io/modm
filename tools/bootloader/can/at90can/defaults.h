// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2010, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  * Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *  * Neither the name of the Roboterclub Aachen e.V. nor the
 *    names of its contributors may be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ROBOTERCLUB AACHEN E.V. ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ROBOTERCLUB AACHEN E.V. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $Id$
 */
// ----------------------------------------------------------------------------

#ifndef	DEFAULTS_H
#define	DEFAULTS_H

#include "config.h"

// ----------------------------------------------------------------------------
// create pagesize identifier
#if	SPM_PAGESIZE == 32
	#define	PAGESIZE_IDENTIFIER		(0)
#elif SPM_PAGESIZE == 64
	#define	PAGESIZE_IDENTIFIER		(1)
#elif SPM_PAGESIZE == 128
	#define	PAGESIZE_IDENTIFIER		(2)
#elif SPM_PAGESIZE == 256
	#define	PAGESIZE_IDENTIFIER		(3)
#else
	#error	Strange value for SPM_PAGESIZE. Check the define!
#endif

// -----------------------------------------------------------------------------
// check defines for the bootloader led
#ifdef BOOT_LED
	#ifndef	BOOT_LED_SET_OUTPUT	
		#define	BOOT_LED_SET_OUTPUT		SET_OUTPUT(BOOT_LED)
	#endif
	
	#ifndef	BOOT_LED_ON
		#define	BOOT_LED_ON				SET(BOOT_LED)
	#endif
	
	#ifndef	BOOT_LED_OFF
		#define	BOOT_LED_OFF			RESET(BOOT_LED)
	#endif
	
	#ifndef	BOOT_LED_TOGGLE	
		#define	BOOT_LED_TOGGLE			TOGGLE(BOOT_LED)
	#endif
#else
	#define	BOOT_LED_SET_OUTPUT			
	#define	BOOT_LED_ON
	#define	BOOT_LED_OFF
	#define	BOOT_LED_TOGGLE
	
	#warning	compiling bootloader without LED support
#endif

// -----------------------------------------------------------------------------
#ifndef	BOOT_INIT
	#define	BOOT_INIT
#endif

#ifndef	BOOTLOADER_TYPE
	#define	BOOTLOADER_TYPE		0
#endif

// -----------------------------------------------------------------------------
// set current version of the bootloader
#define	BOOTLOADER_VERSION		2

// -----------------------------------------------------------------------------
// CAN settings
#ifndef	CAN_BITRATE
	#define	CAN_BITRATE		125
#endif

// ----------------------------------------------------------------------------
// Set a few AVR specific defines

#if defined(__AVR_AT90CAN32__)
	
	#define	RWW_PAGES	96
//	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x95
	#define	SIG_DEVICE	0x81
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#elif defined(__AVR_AT90CAN64__)
	
	#define	RWW_PAGES	224
//	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x96
	#define	SIG_DEVICE	0x81
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#elif defined(__AVR_AT90CAN128__)
	
	#define	RWW_PAGES	480
//	#define	RAMSTART	0x0100
	#define	SIG_FAMILY	0x97
	#define	SIG_DEVICE	0x81
	#define	TIMER_INTERRUPT_FLAG_REGISTER	TIFR1
	
#else
	#error	chosen AVR command is not supported yet!
#endif

// ----------------------------------------------------------------------------
// Timereinstellung fuer aktuelle Taktfrequenz auswaehlen (500ms)
//
// TIMER_PRELOAD = 65536 - (0.5s * F_CPU) / 1024

#if F_CPU == 16000000UL
	#define	TIMER_PRESCALER		(1<<CS12)|(1<<CS10)		// Prescaler = 1024
	#define	TIMER_PRELOAD		57724
#else
	#error	choosen F_CPU not supported yet!
#endif


#endif	// DEFAULTS_H
