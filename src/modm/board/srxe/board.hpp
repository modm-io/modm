/*
 * Copyright (c) 2021, Tomasz Wasilczyk
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

/// @ingroup modm_board_srxe
namespace Board {

using namespace modm::literals;

using SystemClock = modm::platform::SystemClock;

using LedDebug = GpioB0;
using Leds = SoftwareGpioPort<LedDebug>;

namespace Display {

using DC = GpioD6;
using CS = GpioE7;
using RST = GpioG2;

}  // namespace Display

inline void
initialize() {
	SystemClock::enable();

	LedDebug::setOutput();

	enableInterrupts();
}

}  // namespace Board
