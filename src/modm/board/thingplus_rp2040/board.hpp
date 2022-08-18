/*
 *
 * Copyright (c) 2022, Andrey Kunitsyn
 * Copyright (c) 2022, Nikolay Semenov
 *
 * This file is part of the modm project.
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */
// ----------------------------------------------------------------------------

#pragma once

#include <modm/architecture/interface/clock.hpp>
#include <modm/platform.hpp>

using namespace modm::platform;

namespace Board
{
using namespace modm::literals;

/// RP2040 running at 125MHz generated from the external 12MHz crystal
/// @ingroup modm_board_thingplus_rp2040
struct SystemClock
{
	static constexpr uint32_t Frequency = 125_MHz;
	static constexpr uint32_t XOSCFrequency = 12_MHz;
	static constexpr uint32_t PllSysFrequency = Frequency;
	static constexpr uint32_t PllUsbFrequency = 48_MHz;
	static constexpr uint32_t SysPLLMul = 125;
	static constexpr uint32_t UsbPLLMul = 40;
	static constexpr uint32_t RefFrequency = XOSCFrequency;
	static constexpr uint32_t UsbFrequency = PllUsbFrequency;
	static constexpr uint32_t SysFrequency = Frequency;
	static constexpr uint32_t PeriFrequency = SysFrequency;

	static bool inline enable()
	{
		ClockControl::disableResus();
		ClockControl::enableExternalCrystal(XOSCFrequency);
		ClockControl::disableAux<ClockControl::Clock::Sys>();
		ClockControl::disableAux<ClockControl::Clock::Ref>();
		// PLL SYS: 12MHz / 1 = 12MHz * 125 = 1500MHZ / 6 / 2 = 125MHz
		ClockControl::initPll<ClockControl::Pll::Sys, 1, SysPLLMul, 6, 2>();
		// PLL USB: 12MHz / 1 = 12MHz * 40  = 480 MHz / 5 / 2 =  48MHz
		ClockControl::initPll<ClockControl::Pll::Usb, 1, UsbPLLMul, 5, 2>();

		// CLK_REF = XOSC (12MHz) / 1 = 12MHz
		ClockControl::configureClock<ClockControl::Clock::Ref, ClockControl::ClockSrc::Xosc,
									 XOSCFrequency, RefFrequency>();
		// CLK SYS = PLL SYS (125MHz) / 1 = 125MHz
		ClockControl::configureClock<ClockControl::Clock::Sys, ClockControl::ClockSrc::PllSys,
									 PllSysFrequency, SysFrequency>();
		// CLK USB = PLL USB (48MHz) / 1 = 48MHz
		ClockControl::configureClock<ClockControl::Clock::Usb, ClockControl::ClockSrc::PllUsb,
									 PllUsbFrequency, UsbFrequency>();
		// CLK PERI = clk_sys. Used as reference clock for Peripherals. No dividers so just select
		// and enable Normally choose clk_sys or clk_usb
		ClockControl::configureClock<ClockControl::Clock::Peri, ClockControl::ClockSrc::Sys,
									 SysFrequency, PeriFrequency>();

		ClockControl::updateCoreFrequency<Frequency>();
		return true;
	}
};

namespace usb
{
/// @ingroup modm_board_thingplus_rp2040
using Device = Usb;
}

/// @ingroup modm_board_thingplus_rp2040
/// @{
// User LED
using LedBlue = GpioOutput25;
using Led = LedBlue;
using Leds = SoftwareGpioPort<Led>;

using Button = GpioUnused;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Led::setOutput();
}

inline void
initializeUsbFs(uint8_t priority=3)
{
	usb::Device::initialize<SystemClock>(priority);
	usb::Device::connect<>();
}
/// @}

}  // namespace Board
