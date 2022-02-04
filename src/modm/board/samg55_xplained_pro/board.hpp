/*
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

#include <modm/platform.hpp>
using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_samg55_xplained_pro
/// @{
using namespace modm::literals;

struct SystemClock
{
	// Chosen to achieve 120MHz system clock
	static constexpr uint32_t PllAMult = 3662;
	// Chosen to achieve 48MHz USB clock
	static constexpr uint32_t PllBMult = 1465;

	static constexpr uint32_t Frequency = PllAMult * SlowClkFreqHz; // CPU core frequency
	static constexpr uint32_t Usb = PllBMult * SlowClkFreqHz;
	static constexpr uint32_t Mck = Frequency; // Master clock: default used by most peripherals
	// Programmable clocks: optionally used by certain peripherals
	static constexpr uint32_t Pck0 = Mck;
	static constexpr uint32_t Pck1 = Mck;
	static constexpr uint32_t Pck2 = Mck;
	static constexpr uint32_t Pck3 = Mck;
	static constexpr uint32_t Pck4 = Mck;
	static constexpr uint32_t Pck5 = Mck;
	static constexpr uint32_t Pck6 = Mck;
	static constexpr uint32_t Pck7 = Mck;

	static constexpr uint32_t Usart7 = Mck;

	static bool inline
	enable()
	{
		ClockGen::setFlashLatency<Frequency>();
		ClockGen::updateCoreFrequency<Frequency>();
		ClockGen::enableExternal32Khz(false);
		ClockGen::enablePllA<PllAMult>();
		ClockGen::selectMasterClk<MasterClkSource::PLLA_CLK, MasterClkPrescaler::CLK_1>();
		return true;
	}

	static bool inline
	enableUsb()
	{
		ClockGen::enablePllB<PllBMult>();
		// Use PLLB as USB clock source
		PMC->PMC_USB = PMC_USB_USBS;
		return true;
	}
};

using Led = GpioA6;
using Button = GpioA2;
using DebugUart = Usart7;
using TxPin = GpioA28;
using RxPin = GpioA27;

inline void
initialize()
{
	// Turn off the watchdog
	WDT->WDT_MR = WDT_MR_WDDIS_Msk;

	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	DebugUart::initialize<SystemClock, 115200>();
	DebugUart::connect<TxPin::Tx, RxPin::Rx>();

	Led::setOutput(modm::Gpio::Low);

	Button::setInput();
}

inline void
initializeUsbFs()
{
	SystemClock::enableUsb();
	Usb::initialize<Board::SystemClock>();
}
/// @}

} // namespace Board

