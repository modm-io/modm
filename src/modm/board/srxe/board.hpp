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

#include <modm/driver/display/st7586s.hpp>

using namespace modm::platform;

/// @ingroup modm_board_srxe
namespace Board {

using namespace modm::literals;

using SystemClock = modm::platform::SystemClock;

using LedDebug = GpioB0;
using Leds = SoftwareGpioPort<LedDebug>;

namespace spi {

using Sck = GpioB1;
using Mosi = GpioB2;
using Miso = GpioB3;
using SpiMaster = modm::platform::SpiMaster;

}  // namespace spi

namespace DisplayGpio {

using DC = GpioD6;
using CS = GpioE7;
using RST = GpioG2;

}  // namespace DisplayGpio

using Display = modm::St7586s<spi::SpiMaster, DisplayGpio::CS, DisplayGpio::RST, DisplayGpio::DC,
	384, 136>;
extern Display display;

inline void
initialize() {
	SystemClock::enable();

	LedDebug::setOutput();

	spi::SpiMaster::connect<spi::Sck::Sck, spi::Mosi::Mosi, spi::Miso::Miso>();
	/* Display controller datasheet requires minimum 60+140ns clock pulse width (that's 3.5MHz
	 * for 200ns cycle or 5MHz for 280ns), but it seems to work just fine with 8MHz.
	 */
	spi::SpiMaster::initialize<SystemClock, 4_MHz>();
	// Clock is high when inactive, data is sampled on clock trailing edge.
	spi::SpiMaster::setDataMode(spi::SpiMaster::DataMode::Mode3);

	enableInterrupts();

	display.initialize();
}

}  // namespace Board
