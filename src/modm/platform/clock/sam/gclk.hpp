/*
 * Copyright (c) 2019, Ethan Slattery
 * Copyright (c) 2020, Erik Henriksson
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

namespace modm::platform
{

enum class
ClockSource : uint32_t
{
	OSC8M = GCLK_GENCTRL_SRC_OSC8M_Val,
	DFLL48M = GCLK_GENCTRL_SRC_DFLL48M_Val
};

enum class
ClockGenerator : uint32_t
{
	System = 0,
	ExternalCrystal32K = 1,
	ULP32K = 2,
	Internal8M = 3,
};

enum class ClockPeripheral : uint32_t {
	Dfll48 = GCLK_CLKCTRL_ID_DFLL48_Val,
	Fdpll = GCLK_CLKCTRL_ID_FDPLL_Val,
	Fdpll32K = GCLK_CLKCTRL_ID_FDPLL32K_Val,
	Wdt = GCLK_CLKCTRL_ID_WDT_Val,
	Rtc = GCLK_CLKCTRL_ID_RTC_Val,
	Eic = GCLK_CLKCTRL_ID_EIC_Val,
	Usb = GCLK_CLKCTRL_ID_USB_Val,
	Evsys0 = GCLK_CLKCTRL_ID_EVSYS_0_Val,
	Evsys1 = GCLK_CLKCTRL_ID_EVSYS_1_Val,
	Evsys2 = GCLK_CLKCTRL_ID_EVSYS_2_Val,
	Evsys3 = GCLK_CLKCTRL_ID_EVSYS_3_Val,
	Evsys4 = GCLK_CLKCTRL_ID_EVSYS_4_Val,
	Evsys5 = GCLK_CLKCTRL_ID_EVSYS_5_Val,
	Evsys6 = GCLK_CLKCTRL_ID_EVSYS_6_Val,
	Evsys7 = GCLK_CLKCTRL_ID_EVSYS_7_Val,
	Evsys8 = GCLK_CLKCTRL_ID_EVSYS_8_Val,
	Evsys9 = GCLK_CLKCTRL_ID_EVSYS_9_Val,
	Evsys10 = GCLK_CLKCTRL_ID_EVSYS_10_Val,
	Evsys11 = GCLK_CLKCTRL_ID_EVSYS_11_Val,
	SercomXSlow = GCLK_CLKCTRL_ID_SERCOMX_SLOW_Val,
	Sercom0 = GCLK_CLKCTRL_ID_SERCOM0_CORE_Val,
	Sercom1 = GCLK_CLKCTRL_ID_SERCOM1_CORE_Val,
	Sercom2 = GCLK_CLKCTRL_ID_SERCOM2_CORE_Val,
	Sercom3 = GCLK_CLKCTRL_ID_SERCOM3_CORE_Val,
	Sercom4 = GCLK_CLKCTRL_ID_SERCOM4_CORE_Val,
	Sercom5 = GCLK_CLKCTRL_ID_SERCOM5_CORE_Val,
	Tcc0 = GCLK_CLKCTRL_ID_TCC0_TCC1_Val,
	Tcc1 = GCLK_CLKCTRL_ID_TCC0_TCC1_Val,
	Tcc2 = GCLK_CLKCTRL_ID_TCC2_TC3_Val,
	Tc3 = GCLK_CLKCTRL_ID_TCC2_TC3_Val,
	Tc4 = GCLK_CLKCTRL_ID_TC4_TC5_Val,
	Tc5 = GCLK_CLKCTRL_ID_TC4_TC5_Val,
	Tc6 = GCLK_CLKCTRL_ID_TC6_TC7_Val,
	Tc7 = GCLK_CLKCTRL_ID_TC6_TC7_Val,
	Adc = GCLK_CLKCTRL_ID_ADC_Val,
	AcDig = GCLK_CLKCTRL_ID_AC_DIG_Val,
	AcAna = GCLK_CLKCTRL_ID_AC_ANA_Val,
	Dac = GCLK_CLKCTRL_ID_DAC_Val,
	Ptc = GCLK_CLKCTRL_ID_PTC_Val,
	I2s0 = GCLK_CLKCTRL_ID_I2S_0_Val,
	I2s1 = GCLK_CLKCTRL_ID_I2S_1_Val
};

/**
 * Clock management
 *
 * \ingroup modm_platform_gclk
 */
class GenericClockController
{
public:
	static constexpr uint32_t BootFrequency = 1'000'000;

	template< uint32_t Core_Hz>
	static uint32_t
	setFlashLatency();

  template< uint32_t Core_Hz >
	static void
	updateCoreFrequency();

	static bool
	initOsc8MHz(uint32_t waitCycles = 2048);

	static bool
	initExternalCrystal(uint32_t waitCycles = 2048);

	static bool
	initDFLL48MHz(uint32_t waitCycles = 2048);

	static bool
	setSystemClock(
		ClockSource source = ClockSource::OSC8M,
		uint32_t waitCycles = 2048);

	template< ClockPeripheral peripheral >
	static void
	connect(ClockGenerator clockGen);

private:

	enum class
	ClockMux : uint32_t
	{
		DFLL48M = 0,
	};
};

}

#include "gclk_impl.hpp"
