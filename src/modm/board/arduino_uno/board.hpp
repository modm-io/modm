/*
 * Copyright (c) 2016-2018, Niklas Hauser
 * Copyright (c) 2017, Sascha Schade
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
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_arduino_uno
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_arduino_uno
namespace Board
{
using namespace modm::literals;
using SystemClock = modm::platform::SystemClock;

// Arduino Footprint
using A0 = GpioC0;
using A1 = GpioC1;
using A2 = GpioC2;
using A3 = GpioC3;
using A4 = GpioC4;
using A5 = GpioC5;

using D0  = GpioD0;
using D1  = GpioD1;
using D2  = GpioD2;
using D3  = GpioD3;
using D4  = GpioD4;
using D5  = GpioD5;
using D6  = GpioD6;
using D7  = GpioD7;
using D8  = GpioB0;
using D9  = GpioB1;
using D10 = GpioB2;
using D11 = GpioB3;
using D12 = GpioB4;
using D13 = GpioB5;

using Button = GpioUnused;
using LedD13 = D13;

using Leds = SoftwareGpioPort< LedD13 >;

using LoggerDevice = modm::IODeviceWrapper< Uart0, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();

	// Uart0 is connected to onboard USB bridge
	Uart0::connect<D1::Txd, D0::Rxd>();
	Uart0::initialize<SystemClock, 9'600_Bd>();

	enableInterrupts();
}

}

using namespace Board;
extern modm::IOStream serialStream;
