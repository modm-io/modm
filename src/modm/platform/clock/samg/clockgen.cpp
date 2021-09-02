/**
 * Copyright (c) 2021, Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#include "clockgen.hpp"
#include <cmath>


// CMSIS Core compliance
uint32_t modm_fastdata SystemCoreClock(8000000);
modm_weak void SystemCoreClockUpdate() { /* Nothing to update */ }

namespace modm::platform
{
uint16_t modm_fastdata delay_fcpu_MHz = computeDelayMhz(8'000'000);
uint16_t modm_fastdata delay_ns_per_loop = computeDelayNsPerLoop(8'000'000);


static uint16_t divFromMasterClkPrescaler(MasterClkPrescaler pres) {
	switch(pres) {
		case MasterClkPrescaler::CLK_1:
			return 1;
		case MasterClkPrescaler::CLK_2:
			return 2;
		case MasterClkPrescaler::CLK_4:
			return 4;
		case MasterClkPrescaler::CLK_8:
			return 8;
		case MasterClkPrescaler::CLK_16:
			return 16;
		case MasterClkPrescaler::CLK_32:
			return 32;
		case MasterClkPrescaler::CLK_64:
			return 64;
		case MasterClkPrescaler::CLK_3:
			return 3;
		default:
			return 1;
	}
}

void ClockGen::enableExternal32Khz(bool crystal_bypass) {
	if(crystal_bypass) {
		SUPC->SUPC_MR |= SUPC_MR_OSCBYPASS;
	}

	SUPC->SUPC_CR = SUPC_CR_KEY_PASSWD | SUPC_CR_XTALSEL;
}




uint32_t ClockGen::masterClkFrequency() {
	uint32_t mckr = PMC->PMC_MCKR;
	MasterClkSource src = (MasterClkSource)((mckr & PMC_MCKR_CSS_Msk) >> PMC_MCKR_CSS_Pos);
	MasterClkPrescaler pres = (MasterClkPrescaler)((mckr & PMC_MCKR_PRES_Msk) >> PMC_MCKR_PRES_Pos);
	uint32_t div = divFromMasterClkPrescaler(pres);
	switch(src) {
		case MasterClkSource::SLOW_CLK:
			return SlowClkFreqHz / div;
		case MasterClkSource::MAIN_CLK:
			return mainClkFrequency() / div;
		case MasterClkSource::PLLA_CLK:
			return pllAFrequency() / div;
		case MasterClkSource::PLLB_CLK:
			return pllBFrequency() / div;
		default:
			return 0;
	}
}

uint32_t ClockGen::mainClkFrequency() {
	// NOTE: This does not support external main clk. To add this, we'll need
	// a way for user supplied input frequency. For now, this assumes we're
	// using the internal RC for main clock.
	uint32_t moscrcf = (PMC->CKGR_MOR & CKGR_MOR_MOSCRCF_Msk) >> CKGR_MOR_MOSCRCF_Pos;
	if(moscrcf == (uint32_t)MainInternalFreq::Rc8Mhz) {
		return 8'000'000;
	} else if(moscrcf == (uint32_t)MainInternalFreq::Rc16Mhz) {
		return 16'000'000;
	} else if(moscrcf == (uint32_t)MainInternalFreq::Rc24Mhz) {
		return 24'000'000;
	} else {
		return 0;
	}
}

uint32_t ClockGen::pllAFrequency() {
	uint32_t mul = ((PMC->CKGR_PLLAR & CKGR_PLLAR_MULA_Msk) >> CKGR_PLLAR_MULA_Pos) + 1;
	uint32_t freq = SlowClkFreqHz * mul;
	if(PMC->PMC_MCKR & PMC_MCKR_PLLADIV2) {
		freq /= 2;
	}
	return freq;
}

uint32_t ClockGen::pllBFrequency() {
	uint32_t mul = ((PMC->CKGR_PLLBR & CKGR_PLLBR_MULB_Msk) >> CKGR_PLLBR_MULB_Pos) + 1;
	uint32_t freq = SlowClkFreqHz * mul;
	if(PMC->PMC_MCKR & PMC_MCKR_PLLBDIV2) {
		freq /= 2;
	}
	return freq;
}


void ClockGen::setMainInternalFreq(MainInternalFreq freq) {
	PMC->CKGR_MOR =
		(PMC->CKGR_MOR & ~(CKGR_MOR_MOSCRCF_Msk)) |
		CKGR_MOR_MOSCRCF((uint32_t)freq) |
		CKGR_MOR_KEY_PASSWD;
}

} // namespace modm::platform