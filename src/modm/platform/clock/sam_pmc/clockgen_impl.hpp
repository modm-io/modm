/*
 * Copyright (c) 2021 Jeff McBride
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

namespace modm::platform {

template <MasterClkSource src, MasterClkPrescaler pres>
void ClockGen::selectMasterClk() {
	// Datasheet says when selecting PLL as source, write PRES first, otherwise
	// write CSS first.
	if(src == MasterClkSource::PLLA_CLK || src == MasterClkSource::PLLB_CLK) {
		PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_PRES_Msk) | PMC_MCKR_PRES((uint32_t)pres);
		while(!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}
		PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS((uint32_t)src);
		while(!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}
	} else {
		PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_CSS_Msk) | PMC_MCKR_CSS((uint32_t)src);
		while(!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}
		PMC->PMC_MCKR = (PMC->PMC_MCKR & ~PMC_MCKR_PRES_Msk) | PMC_MCKR_PRES((uint32_t)pres);
		while(!(PMC->PMC_SR & PMC_SR_MCKRDY)) {}
	}
}

template<uint32_t multiplier>
void ClockGen::enablePllA(uint32_t wait_cycles) {
    static_assert(multiplier > 8 && multiplier < 7502, "Valid PLL MUL range is 9-7501");
    PMC->CKGR_PLLAR =
        CKGR_PLLAR_MULA(multiplier-1) |
        CKGR_PLLAR_PLLACOUNT(wait_cycles) |
        CKGR_PLLAR_PLLAEN(1);
    // Wait for lock bit
    while(!(PMC->PMC_SR & PMC_SR_LOCKA)) {}
}

template<uint32_t multiplier>
void ClockGen::enablePllB(uint32_t wait_cycles) {
    static_assert(multiplier > 8 && multiplier < 7502, "Valid PLL MUL range is 9-7501");
    PMC->CKGR_PLLBR =
        CKGR_PLLBR_MULB(multiplier-1) |
        CKGR_PLLBR_PLLBCOUNT(wait_cycles) |
        CKGR_PLLBR_PLLBEN(1);
    // Wait for lock bit
    while(!(PMC->PMC_SR & PMC_SR_LOCKB)) {}
}

template<uint32_t Core_Hz>
void ClockGen::updateCoreFrequency() {
	SystemCoreClock = Core_Hz;
	delay_fcpu_MHz = computeDelayMhz(Core_Hz);
	delay_ns_per_loop = computeDelayNsPerLoop(Core_Hz);
}

template <uint32_t Core_Hz>
void ClockGen::setFlashLatency() {
	uint32_t fws = 0;
	if(Core_Hz <= 20'000'000) {
		fws = 0;
	} else if(Core_Hz <= 40'000'000) {
		fws = 1;
	} else if (Core_Hz <= 60'000'000) {
		fws = 2;
	} else if (Core_Hz <= 80'000'000) {
		fws = 3;
	} else if (Core_Hz <= 100'000'000) {
		fws = 4;
	} else {
		fws = 5;
	}
	EFC->EEFC_FMR = (EFC->EEFC_FMR & ~EEFC_FMR_FWS_Msk) | EEFC_FMR_FWS(fws);
}

template< ClockPeripheral peripheral >
void ClockGen::enable() {
	constexpr uint32_t id = (uint32_t)peripheral;
	if constexpr (id < 32) {
		PMC->PMC_PCER0 = (1<<id);
	} else {
		PMC->PMC_PCER1 = (1<<(id-32));
	}
}

template< ClockPeripheral peripheral >
bool ClockGen::isEnabled() {
	constexpr uint32_t id = (uint32_t)peripheral;
	if constexpr (id < 32) {
		return PMC->PMC_PCSR0 & (1<<id);
	} else {
		return PMC->PMC_PCSR1 & (1<<(id-32));
	}
}

template< ClockPeripheral peripheral >
void ClockGen::disable() {
	constexpr uint32_t id = (uint32_t)peripheral;
	if constexpr (id < 32) {
		PMC->PMC_PCDR0 = (1<<id);
	} else {
		PMC->PMC_PCDR1 = (1<<(id-32));
	}
}


} // namespace modm::platform
