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

#pragma once

#include <stdint.h>
#include "../device.hpp"
#include <modm/architecture/interface/delay.hpp>

extern uint32_t SystemCoreClock;

namespace modm::platform {

enum class MasterClkSource : uint32_t {
	SLOW_CLK = PMC_MCKR_CSS_SLOW_CLK_Val,
	MAIN_CLK = PMC_MCKR_CSS_MAIN_CLK_Val,
	PLLA_CLK = PMC_MCKR_CSS_PLLA_CLK_Val,
	PLLB_CLK = PMC_MCKR_CSS_PLLB_CLK_Val
};

enum class MasterClkPrescaler : uint32_t {
	CLK_1 = 0,
	CLK_2,
	CLK_4,
	CLK_8,
	CLK_16,
	CLK_32,
	CLK_64,
	CLK_3
};

enum class MainInternalFreq : uint32_t {
	Rc8Mhz = 0,
	Rc16Mhz,
	Rc24Mhz
};

static constexpr uint32_t SlowClkFreqHz = 32'768;

/**
 * Clock Generator Controller for SAMG5x devices
 *
 * This class provides control of the clock generation for the CPU and
 * peripherals.
 *
 * There are two source clocks: a "slow" 32kHz clock which can be sourced from
 * an external oscillator/crystal or internal RC oscillator, and a "main" clock which
 * can be sourced from the 8/16/24MHz selectable internal RC oscillator, or from
 * an external oscillator/crystal.
 *
 * In addition, there are two PLLs (PLLA, and PLLB), which run off of the slow
 * clock. PLLB is used to provide a 48MHz USB clock (external slow crystal is
 * required for USB), and PLLA can (optionally) be used to provide a faster
 * clock for the CPU and peripherals.
 *
 * This class does not allow full control of all possible clock configurations.
 *
 * @author		Jeff McBride
 * @ingroup		modm_platform_clockgen
 */
class ClockGen {
public:
	/**
	 * Select external source for slow clock
	 *
	 * \param	crystal_bypass
	 *      Set true to enable use of external oscillator input on XIN, instead
	 *      of crystal.
	 */
	static void enableExternal32Khz(bool crystal_bypass = false);

	template<uint32_t multiplier>
	static void enablePllA(uint32_t wait_cycles = 50);

	template<uint32_t multiplier>
	static void enablePllB(uint32_t wait_cycles = 50);

	template <MasterClkSource src, MasterClkPrescaler pres>
	static void selectMasterClk();

	static void setMainInternalFreq(MainInternalFreq freq);

	template<uint32_t Core_Hz>
	static void setFlashLatency();

	template<uint32_t Core_Hz>
	static void updateCoreFrequency();

	/** Returns the configured frequency of the main clock */
	static uint32_t mainClkFrequency();

	/** Returns the configured frequency of the master clock */
	static uint32_t masterClkFrequency();

	/** Returns the configured frequency of PLL A output */
	static uint32_t pllAFrequency();

	/** Returns the configured frequency of PLL B output */
	static uint32_t pllBFrequency();
};

} // namespace modm::platform

#include "clockgen_impl.hpp"