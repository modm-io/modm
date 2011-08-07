// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Roboterclub Aachen e.V. nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
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

#include "registers.h"

#include "clock.hpp"

// Ref: UM10237 LPC24XX User manual Rev. 04 - 26. August 2009
// Fcco = (2*M*Fin)/N
// allowed Fcco range 275 MHz to 550 MHz
// Fin = 12MHz @ Olimex LPC-E2468 board
// Fcpu max = 72MHz
// for USB usage Fcco target values { 288 MHz, 384 MHz 480 MHz }

// configuration:
//  Fcco = 288 MHz
//  Fcpu =  72 MHz
//  Fin  =  12 MHz
//  M=12  N=1

#define PLL_MValue		11	// multiplier M = 12
#define PLL_NValue		0	// divider N = 1
#define CCLKDivValue	3	// divide Fcco by 4 --> cclk   = 72  MHz
#define USBCLKDivValue	5	// divide Fcco by 6 --> usbclk = 48  MHz

#define PLLSTAT_PLLE	(1 << 24)
#define PLLSTAT_PLLC	(1 << 25)
#define PLLSTAT_PLOCK	(1 << 26)

void
xpcc::lpc::Clock::initialize()
{
	/*if ((PLLSTAT & (PLLSTAT_PLLC)) != 0) {
		// Enable PLL, disconnected
		PLLCON = 1;
		PLLFEED = 0xaa;
		PLLFEED = 0x55;
	}
	
	// Disable PLL, disconnected
	PLLCON = 0;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	// Set SCS.OSCRANGE = 0  (Fosc = 1MHz..20MHz) and enable main OSC
	SCS = SCS & ~(1 << 4);
	SCS = SCS |  (1 << 5);
	while (!(SCS & 0x40)) {
		// Wait until main OSC is usable
	}

	// select main OSC as the PLL clock source
	CLKSRCSEL = 0x1;

	PLLCFG = (PLL_MValue & 0x7FFF) | ((PLL_NValue & 0xFF) << 16);
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	
	// Enable PLL, disconnected
	PLLCON = 1;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;
	
	// Set clock divider
	CCLKCFG = CCLKDivValue;

	while ((PLLSTAT & PLLSTAT_PLOCK) == 0) {
		// wait for PLL lock
	};
	
	// TODO
	volatile unsigned long MValue = PLLSTAT & 0x00007FFF;
	volatile unsigned long NValue = (PLLSTAT & 0x00FF0000) >> 16;
	while ((MValue != PLL_MValue) && (NValue != PLL_NValue)) {
		
	}
	
	// enable and connect
	PLLCON = 3;
	PLLFEED = 0xaa;
	PLLFEED = 0x55;

	while ( ((PLLSTAT & (PLLSTAT_PLLC)) == 0) ) {
		// Check connect bit status
	}*/
}
