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

// AVR AT90CAN128 breakout board AL-AVREB_CAN
// https://www.alvidi.de/products/DE/AVR_Entwicklungsboards/avr_modul_avreb_can.php
// Tested with 16MHz external crystal

#ifndef MODM_AL_AVREB_CAN_HPP
#define MODM_AL_AVREB_CAN_HPP

#include <modm/platform.hpp>
#include <modm/io/iostream.hpp>

using namespace modm::platform;

/// @ingroup modm_board_al_avreb_can
namespace Board
{
using namespace modm::literals;
using SystemClock = modm::platform::SystemClock;

using Led0 = GpioInverted<GpioOutputF0>;
using Led1 = GpioInverted<GpioOutputF1>;
using Led2 = GpioInverted<GpioOutputF2>;
using Led3 = GpioInverted<GpioOutputF3>;

using Button = GpioUnused;
using Leds = SoftwareGpioPort< Led3, Led2, Led1, Led0 >;

using LoggerDevice = modm::IODeviceWrapper< Uart1, modm::IOBuffer::BlockIfFull >;

inline void
initialize()
{
	SystemClock::enable();

	Uart1::connect<GpioD3::Txd, GpioD2::Rxd>();
	Uart1::initialize<SystemClock, 38'400_Bd>();

	enableInterrupts();
}

}

using namespace Board;
extern modm::IOStream serialStream;

#endif	// MODM_ARDUINO_UNO_HPP
