// coding: utf-8
// ----------------------------------------------------------------------------
/* Copyright (c) 2011, Roboterclub Aachen e.V.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *	 * Redistributions of source code must retain the above copyright
 *	   notice, this list of conditions and the following disclaimer.
 *	 * Redistributions in binary form must reproduce the above copyright
 *	   notice, this list of conditions and the following disclaimer in the
 *	   documentation and/or other materials provided with the distribution.
 *	 * Neither the name of the Roboterclub Aachen e.V. nor the
 *	   names of its contributors may be used to endorse or promote products
 *	   derived from this software without specific prior written permission.
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

#include <stdint.h>

#include "../device.h"
#include "../../common/default_handler.h"

// ----------------------------------------------------------------------------
static void
initializePll(void)
{
	// 1. Enable the Main Oscillator:
	// SCK = 1/32768 = 30.51 uSecond
	// OSCOUNT = 6
	// Start up time = 8 * OSCOUNT / SCK = 56 * 30.51 = 1,46 ms
	AT91C_BASE_PMC->PMC_MOR = ((6 << 8) & AT91C_CKGR_OSCOUNT) | AT91C_CKGR_MOSCEN;
	while ((AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MOSCS) == 0) {
		// wait unitl the main oscillator has stabilized
	}
	
	// 2. Checking the Main Oscillator Frequency (Optional)
	// 3. Set the PLL and Divider:
	// - div by 14 Fin = 1.3165 = (18,432 / 14)
	// - Mul 72+1: Fout =	96.1097 = (3,6864 * 73)
	// for 96 MHz the error is 0.11%
	// Field out NOT USED = 0
	// PLLCOUNT pll startup time estimate at : 0.844 ms
	// PLLCOUNT 28 = 0.000844 / (1 / 32768)
	AT91C_BASE_PMC->PMC_PLLR =
			((AT91C_CKGR_DIV & 14) |
			 (AT91C_CKGR_PLLCOUNT & (28 << 8)) |
			 (AT91C_CKGR_MUL & (72 << 16)));
	
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_LOCK)) {
		// wait until PLL is locked
	}
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY)) {
		// wait until Master Clock is ready
	}
	
	// 4. Selection of Master Clock and Processor Clock
	// select the PLL clock divided by 2
	AT91C_BASE_PMC->PMC_MCKR =  AT91C_PMC_PRES_CLK_2;
	while(!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY)) {
		// wait until Master Clock is ready
	}
	
	// Use PLL as clock source
	AT91C_BASE_PMC->PMC_MCKR |= AT91C_PMC_CSS_PLL_CLK;
	while (!(AT91C_BASE_PMC->PMC_SR & AT91C_PMC_MCKRDY)) {
		// wait until Master Clock is ready
	}
}

// ----------------------------------------------------------------------------
/* 
 * @brief	Hardware start-up function
 * 
 */
void
_at91Init(void)
{
	// Watchdog Disable
	AT91C_BASE_WDTC->WDTC_WDMR = AT91C_WDTC_WDDIS;
	
	// TODO Enable User Reset and set its minimal assertion to 960 us
	AT91C_BASE_RSTC->RSTC_RMR = AT91C_RSTC_URSTEN | (0x4<<8) | (unsigned int)(0xA5<<24);
	
	// Initialize Flash Wait States
	// 
	// See Datasheet AT91SAM7S256, Rev. C:
	// 40.9.1.1 EFC: Embedded Flash Access Time 1
	//   The embedded Flash maximum access time is 20 MHz (instead of 30 MHz)
	//   at zero Wait State (FWS = 0).
	//   The maximum operating frequency with one Wait State (FWS = 1) is
	//   48.1 MHz (instead of 55 MHz). Above 48.1 MHz and up to 55 MHz,
	//   two Wait States (FWS = 2) are required.
	uint32_t flashParameter = 0;
	
#if F_CPU < 20000000UL
	flashParameter |= AT91C_MC_FWS_0FWS;
#elif F_CPU < 48100000UL
	flashParameter |= AT91C_MC_FWS_1FWS;
#else
	flashParameter |= AT91C_MC_FWS_2FWS;
#endif
	
	// Set cycles-per-microsecond
	// as per datasheet,
	//	- NVM bits require a setting of F_CPU / 1000
	//	- general flash write requires a setting of 1.5 * F_CPU / 1000
	// (here we're extra conservative setting clock cycles equal to 2us)
	flashParameter |= ((F_CPU * 2 / 1000L) << 16) & AT91C_MC_FMCN;
	
#if defined(__AT91SAM7S512__) || defined(__AT91SAM7X512__)
	AT91C_BASE_EFC0->EFC_FMR = flashParameter;
	AT91C_BASE_EFC1->EFC_FMR = flashParameter;
#else
	AT91C_BASE_MC->MC_FMR = flashParameter;
#endif
	
	initializePll();
	
	// Set up the default interrupts handler vectors
	AT91C_BASE_AIC->AIC_SPU = (uintptr_t) defaultSpuriousHandler;
	AT91C_BASE_AIC->AIC_SVR[0] = (uintptr_t) defaultFiqHandler;
	for (int i = 1; i < 31; i++) {
		AT91C_BASE_AIC->AIC_SVR[i] = (uintptr_t) defaultIrqHandler;
	}
}
