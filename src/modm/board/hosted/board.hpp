/*
 * Copyright (c) 2026, Niklas Hauser
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
#include <modm/architecture/interface/delay.hpp>
#include <modm/debug/logger.hpp>
/// @ingroup modm_board_hosted
#define MODM_BOARD_HAS_LOGGER

using namespace modm::platform;

/// @ingroup modm_board_hosted
namespace Board
{
using namespace modm::literals;

/// There are no LEDs and buttons on a computer
using Led = GpioUnused;
using Button = GpioUnused;

struct Leds
{
	static constexpr uint8_t width = 1;
	static void setOutput() {}
	static void setOutput(bool) {}
	static void set() {}
	static void reset() {}
	static void toggle() {}
	static void write(uint8_t) {}
	static uint8_t read() { return 0; }
};

/// The logger prints to the standard output
inline void
initialize()
{
}

} // namespace Board
