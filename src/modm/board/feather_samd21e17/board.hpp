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

using ARef 	= GpioA03;
using A0 		= GpioA02;
using A1 		= GpioA06;
using A2 		= GpioA07;
using A3 		= GpioA04;
using A4 		= GpioA05;
using A5 		= GpioA09;

using Sck 	= GpioA15;
using Mosi 	= GpioA14;
using Miso 	= GpioA08;

using Rx	= GpioA11;
using Tx	= GpioA10;

using D13 = GpioA17;
using D12 = GpioA19;
using D11 = GpioA16;
using D10 = GpioA18;

using Sda  = GpioA23;
using Scl  = GpioA22;

// For RFM69 / LoRa boards
using RadioIrq  = A5;
using RadioCs  	= A2;

// This is the red LED by the USB jack.
using Led = D13;
using TxLed = GpioA27;

/// samd21e17 running at 48MHz generated from the external 32.768 KHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 48_MHz;

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

using LoggerDevice = modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull >;
using Leds = SoftwareGpioPort< Led >;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();
	Uart0::connect<Rx::Pad3, Tx::Pad2>();
	Uart0::initialize<SystemClock, 115'200_Bd>();

	Led::setOutput(modm::Gpio::Low);
}

} // Board namespace
