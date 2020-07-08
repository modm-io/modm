/*
 * Copyright (c) 2016-2017, Sascha Schade
 * Copyright (c) 2017-2018, Niklas Hauser
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

#include <modm/debug/logger.hpp>
#include <modm/platform.hpp>
#include <modm/architecture/interface/clock.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;


/// @ingroup modm_board_feather_m0
namespace Board
{
	using namespace modm::literals;

/// samd21g18a running at 48MHz generated from the external 32.768 KHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 48_MHz;
	// static constexpr uint32_t Ahb  = Frequency;
	// static constexpr uint32_t Apba = Frequency;
	// static constexpr uint32_t Apbb = Frequency;
	// static constexpr uint32_t Apbc = Frequency;

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


namespace debug_logger
{
	using Rx = GpioInputA11;
	using Tx = GpioOutputA10;
}

using LoggerDevice = modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull >;

using LedD13 = GpioInverted<GpioOutputA17>;
// using Leds = SoftwareGpioPort< LedD13 >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();
	Uart0::connect<debug_logger::Rx::Pad3, debug_logger::Tx::Pad2>();
	Uart0::initialize<SystemClock, 9'600_Bd>();

	LedD13::setOutput(modm::Gpio::Low);
}

} // Board namespace
