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

#include <modm/architecture/interface/clock.hpp>
#include <modm/debug/logger.hpp>
#include <modm/platform.hpp>
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_feather_m0
namespace Board
{

using namespace modm::literals;

using ARef	= GpioA03;
using A0	= GpioA02;
using A1	= GpioB08;
using A2	= GpioB09;
using A3	= GpioA04;
using A4	= GpioA05;
using A5	= GpioB02;

using Sck 	= GpioB11;
using Mosi 	= GpioB10;
using Miso 	= GpioA12;

using Rx	= GpioA11;
using Tx	= GpioA10;

using D13 = GpioA17;
using D12 = GpioA19;
using D11 = GpioA16;
using D10 = GpioA18;
using D9  = GpioA07;
using D6  = GpioA20;
using D5  = GpioA15;

using Sda  = GpioA23;
using Scl  = GpioA22;

// For RFM69 / LoRa boards
using RadioRst = GpioA08;
using RadioIrq = GpioA09;
using RadioCs = GpioA06;

// This is the red LED by the USB jack.
using Led = D13;

/// samd21g18a running at 48MHz generated from the external 32.768 KHz crystal
struct SystemClock
{
	static constexpr uint32_t Frequency = 48_MHz;
	static constexpr uint32_t Usb = 48_MHz;
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

using LoggerDevice = modm::IODeviceWrapper<Uart0, modm::IOBuffer::BlockIfFull>;
using Leds = Led;

inline void
initialize()
{
	SystemClock::enable();
	SysTickTimer::initialize<SystemClock>();
	Uart0::connect<Rx::Rx, Tx::Tx>();
	Uart0::initialize<SystemClock, 115'200_Bd>();

	Led::setOutput(modm::Gpio::Low);
}

inline void
initializeUsbFs()
{
	modm::platform::Usb::initialize<Board::SystemClock>();
	modm::platform::Usb::connect<GpioA24::Dm, GpioA25::Dp>();
}

}  // namespace Board
