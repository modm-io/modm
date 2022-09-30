/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
 * Copyright (c) 2022, Christopher Durand
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
#include <modm/architecture/interface/clock.hpp>

using namespace modm::platform;

namespace Board
{
/// @ingroup modm_board_samd21_mini
/// @{
using namespace modm::literals;

struct SystemClock
{
	static constexpr uint32_t Dfll48m = 48_MHz;
	static constexpr uint32_t Xosc32k = 32768_Hz;

	static constexpr uint32_t Frequency = Dfll48m;
	static constexpr uint32_t Usb = Dfll48m;

	static constexpr auto ClockGen32kHz = ClockGenerator::Generator2;

	static bool inline
	enable()
	{
		// Configure GCLK generator 2 with external 32k crystal source
		GenericClockController::enableExternalCrystal32k(Xosc32StartupTime::Start_500ms);
		GenericClockController::enableGenerator<ClockGen32kHz, ClockSource::Xosc32k>();

		// generate 48 MHz from 32768 Hz crystal reference
		GenericClockController::connect<ClockPeripheral::Dfll48>(ClockGen32kHz);
		GenericClockController::enableDfll48mClosedLoop<Xosc32k>();

		GenericClockController::setFlashLatency<Frequency>();
		GenericClockController::setSystemClock<ClockSource::Dfll48m>();
		GenericClockController::updateCoreFrequency<Frequency>();

		GenericClockController::connect<ClockPeripheral::Usb>(ClockGenerator::System);
		return true;
	}
};

using ARef	= GpioA03;
using A0	= GpioA02;
using A1	= GpioB08;
using A2	= GpioB09;
using A3	= GpioA04;
using A4	= GpioA05;
using A5	= GpioB02;

using D0  = GpioA11;
using D1  = GpioA10;
using D2  = GpioA14;
using D3  = GpioA09;
using D4  = GpioA08;
using D5  = GpioA15;
using D6  = GpioA20;
using D7  = GpioA21;
using D8  = GpioA06;
using D9  = GpioA07;
using D10 = GpioA18;
using D11 = GpioA16;
using D12 = GpioA19;
using D13 = GpioA17;

// User LED (inverted, because connected to 3V3)
using LedD13 = GpioA17;
using LedTx = GpioA27;
using LedRx = GpioB03;
using Leds = LedTx;

using Button = GpioUnused;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();

	Leds::setOutput(modm::Gpio::Low);
}

inline void
initializeUsbFs(uint8_t priority = 3)
{
	modm::platform::Usb::initialize<Board::SystemClock>(priority);
	modm::platform::Usb::connect<GpioA24::Dm, GpioA25::Dp>();
}
/// @}

} // Board namespace
