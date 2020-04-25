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
/// @ingroup modm_board_mega_2560_pro
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_mega_2560_pro
namespace Board
{
using namespace modm::literals;
using SystemClock = modm::platform::SystemClock;

using D0  = GpioE0;
using D1  = GpioE1;
using D2  = GpioE4;
using D3  = GpioE5;
using D4  = GpioG5;
using D5  = GpioE3;
using D6  = GpioH3;
using D7  = GpioH4;
using D8  = GpioH5;
using D9  = GpioH6;
using D10 = GpioB4;
using D11 = GpioB5;
using D12 = GpioB6;
using D13 = GpioB7;
using D14 = GpioJ1;
using D15 = GpioJ0;
using D16 = GpioH1;
using D17 = GpioH0;
using D18 = GpioD3;
using D19 = GpioD2;
using D20 = GpioD1;
using D21 = GpioD0;
using D22 = GpioA0;
using D23 = GpioA1;
using D24 = GpioA2;
using D25 = GpioA3;
using D26 = GpioA4;
using D27 = GpioA5;
using D28 = GpioA6;
using D29 = GpioA7;
using D30 = GpioC7;
using D31 = GpioC6;

using A0 = GpioF0;
using A1 = GpioF1;
using A2 = GpioF2;
using A3 = GpioF3;
using A4 = GpioF4;
using A5 = GpioF5;
using A6 = GpioF6;
using A7 = GpioF7;
using A8 = GpioK0;
using A9 = GpioK1;
using A10 = GpioK2;
using A11 = GpioK3;
using A12 = GpioK4;
using A13 = GpioK5;
using A14 = GpioK6;
using A15 = GpioK7;

using D32 = GpioC5;
using D33 = GpioC4;
using D34 = GpioC3;
using D35 = GpioC2;
using D36 = GpioC1;
using D37 = GpioC0;
using D38 = GpioD7;
using D39 = GpioG2;
using D40 = GpioG1;
using D41 = GpioG0;
using D42 = GpioL7;
using D43 = GpioL6;
using D44 = GpioL5;
using D45 = GpioL4;
using D46 = GpioL3;
using D47 = GpioL2;
using D48 = GpioL1;
using D49 = GpioL0;
using D50 = GpioB3;
using D51 = GpioB2;
using D52 = GpioB1;
using D53 = GpioB0;

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
	Uart0::initialize<SystemClock, 38'400_Bd>();

	enableInterrupts();
}

}

using namespace Board;
extern modm::IOStream serialStream;
