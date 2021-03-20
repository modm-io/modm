/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
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


/// @ingroup modm_board_samd21_mini
namespace Board
{
using namespace modm::literals;

struct SystemClock
{
	static constexpr uint32_t Frequency = 48_MHz;
	// static constexpr uint32_t Ahb  = Frequency;
	// static constexpr uint32_t Apba = Frequency;
	// static constexpr uint32_t Apbb = Frequency;
	// static constexpr uint32_t Apbc = Frequency;

	static constexpr uint32_t Usb = 48_MHz;
	// static constexpr uint32_t Adc = Apb2;

	// static constexpr uint32_t SercomSlow = Apb2;
	// static constexpr uint32_t Sercom0 = Apb2;
	// static constexpr uint32_t Sercom1 = Apb2;
	// static constexpr uint32_t Sercom2 = Apb2;
	// static constexpr uint32_t Sercom3 = Apb2;
	// static constexpr uint32_t Sercom4 = Apb2;
	// static constexpr uint32_t Sercom5 = Apb2;

	// static constexpr uint32_t Apb1Timer = Apb1 * 2;
	// static constexpr uint32_t Apb2Timer = Apb2 * 1;
	// static constexpr uint32_t Timer1  = Apb2Timer;
	// static constexpr uint32_t Timer2  = Apb1Timer;
	// static constexpr uint32_t Timer3  = Apb1Timer;
	// static constexpr uint32_t Timer4  = Apb1Timer;

	static bool inline
	enable()
	{
		GenericClockController::setFlashLatency<Frequency>();
		GenericClockController::initExternalCrystal();
		GenericClockController::initDFLL48MHz();
		GenericClockController::initOsc8MHz();
		GenericClockController::setSystemClock(ClockSource::DFLL48M);
		GenericClockController::updateCoreFrequency<Frequency>();
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
initializeUsbFs()
{
	modm::platform::Usb::initialize<Board::SystemClock>();
	modm::platform::Usb::connect<GpioA24::Dm, GpioA25::Dp>();
}

} // Board namespace
